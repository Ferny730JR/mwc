#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

#include "mwc_cmdl.h"
#include "parallel_helpers.h"

#define BUFFER_SIZE 65536

/* Global variables */
sem_t *empty, **full; //Declare semaphores to be used
char **data; // Buffer queue

/* Struct to hold command line options information */
struct options {
    bool provided_options;
    bool count_chars;
    bool count_lines;
    bool count_words;
    int num_threads;
};

/* Struct to consumer thread information */
typedef struct consumer_data {
    int consumer_id;              /**< Friendly thread ID. */
	unsigned long num_elements;   /**< Size of the buffer it received. */
	atomic_bool isAlive;          /**< Determines if the thread is alive. */
	atomic_bool busy;             /**< Checks if thread is busy computing. */

    bool count_words;             /** Counting words option */
    bool carry;                   /** Check if carrying the word of previous buffer */

    unsigned long line_result;    /**< Number of lines the thread counted. */
    unsigned long char_result;    /**< Number of chars the thread counted. */
    unsigned long word_result;    /**< Number of words the thread counted. */
} consumer_data;

/* Struct to hold producer thread information */
typedef struct producer_data {
	int num_threads;              /**< Number of threads there are. */
	char *input_file;             /**< Name of file to process */
	consumer_data *tdata;         /**< Consumer thread information. */
    bool encountered_error;       /**< If producer encountered error. */
} producer_data;

/* Struct to hold file count information */
typedef struct count_information {
    unsigned long total_chars;
    unsigned long total_lines;
    unsigned long total_words;
    bool encountered_error;
} count_information;

/* Function Declarations */
static void
count_chars(char *input_file, count_information *count_info);

static void
count_file(char *filename, count_information *count_info, struct options opt);

static void *
producer(void *arg);

static void *
consumer(void *arg);

static int
get_num_threads(struct mwc_args_info args_info);

static char **
collect_unnamed_options(struct mwc_args_info *ggostruct, int *num_files);

static void 
kill_consumers(producer_data *pdata, sem_t **full);

void
init_default_options(struct options *opt)
{
    opt->provided_options = false;
    opt->count_chars = false;
    opt->count_lines = false;
    opt->count_words = false;
    opt->num_threads = 0;
}

void
toggle_options(struct options *opt)
{
    if(opt->provided_options == false) {
        opt->count_chars = true;
        opt->count_lines = true;
        opt->count_words = true;
    }
}


int
main(int argc, char *argv[])
{
    struct mwc_args_info     args_info;
    struct options           opt;
    char                     **input_files;
    int                      num_input = 0;

    init_default_options(&opt);

    /*==========================================
    |  Parse Command Line Options              |
    ==========================================*/

    if(mwc_cmdline_parser(argc, argv, &args_info) != 0) {
        exit(EXIT_FAILURE);
    }

    if(args_info.characters_given) {
        opt.count_chars = true;
        opt.provided_options = true;
    }

    if(args_info.lines_given) {
        opt.count_lines = true;
        opt.provided_options = true;
    }

    if(args_info.words_given) {
        opt.count_words = true;
        opt.provided_options = true;
    }

    opt.num_threads = get_num_threads(args_info);

    input_files = collect_unnamed_options(&args_info, &num_input);

    toggle_options(&opt);

    mwc_cmdline_parser_free(&args_info);

    /*==========================================
    |  Begin Main Computations                 |
    ==========================================*/

    for(int i = 0; i < num_input; i++) {
        count_information count_info = {.total_chars=0, .total_lines=0, .total_words=0, .encountered_error=false};
        if(opt.count_chars) {
            count_chars(input_files[i], &count_info);
        }
        if(opt.count_lines || opt.count_words) {
            count_file(input_files[i], &count_info, opt);
        }
        if(count_info.encountered_error) {
            free(input_files[i]);
            continue;
        }
        if(opt.count_words) {
            printf("%lu ",count_info.total_words);
        }
        if(opt.count_lines) {
            printf("%lu ",count_info.total_lines);
        }
        if(opt.count_chars) {
            printf("%lu ",count_info.total_chars);
        }
        printf("%s\n",input_files[i]);
        free(input_files[i]);
    }
    free(input_files);

    return 0;
}


static void
count_chars(char *input_file, count_information *count_info)
{
    struct stat sb;

    if(stat(input_file, &sb) == -1) {
        /* Signal to main thread that error was encountered */
        count_info->encountered_error = true;

        /* Print error message to stdout */
        char error_output[1000];
        sprintf(error_output, "mwc: %s: open", input_file);
        perror(error_output);
    } else {
        count_info->encountered_error = false;
        count_info->total_chars = (long)sb.st_size;
    }
}


static void
count_file(char *filename, count_information *count_info, struct options opt)
{
    /* Pre processing, if an error was encountered earlier dont proceed */
    if(count_info->encountered_error) {
        return;
    }

    /* producer & consumer threads, respectively */
    pthread_t ptid, *ctids;

	/* Clear existing semaphores */
	sem_unlink("empty_sem");
	for (int i = 0; i < opt.num_threads; i++) {
		char sem_name[20];
		sprintf(sem_name, "full_sem_%d", i);
		sem_unlink(sem_name);
	}

    /* Initialize semaphores */
    empty = sem_open("empty_sem", O_CREAT, 0644, opt.num_threads);
    full = (sem_t **)malloc(opt.num_threads * sizeof(sem_t *));
    for (int i = 0; i < opt.num_threads; i++) {
        char sem_name[20];
        sprintf(sem_name, "full_sem_%d", i);
        full[i] = sem_open(sem_name, O_CREAT, 0644, 0); // Create a separate semaphore for each consumer
    }

    /* Allocate memory for buffer queue */
    data = (char **)malloc(opt.num_threads * sizeof(char *));
    for (int i = 0; i < opt.num_threads; i++) {
        data[i] = (char *)malloc(BUFFER_SIZE * sizeof(char));
    }

	/* Create consumer/producer data */
	consumer_data *tdata = (consumer_data *)malloc(opt.num_threads * sizeof(consumer_data));
	producer_data pdata = {.input_file=filename,
	                       .num_threads=opt.num_threads,
						   .tdata = tdata,
                           .encountered_error = false};

    /* Create consumer threads */
    ctids = (pthread_t *)malloc(opt.num_threads * sizeof(pthread_t));
    for (int i = 0; i < opt.num_threads; i++) {
        tdata[i].consumer_id = i;
		tdata[i].isAlive = true;
		tdata[i].busy = false;
        tdata[i].count_words = opt.count_words;
        tdata[i].num_elements = 0;
        tdata[i].char_result = 0;
        tdata[i].line_result = 0;
        tdata[i].word_result = 0;
        tdata[i].carry = false;
        pthread_create(&ctids[i], NULL, consumer, (void *)&tdata[i]);
    }

    /* Create producer thread */
    pthread_create(&ptid, NULL, producer, (void *)&pdata);

    /* Join producer thread */
    pthread_join(ptid, NULL);

    /* Join consumer threads */
    for (int i = 0; i < opt.num_threads; i++) {
        pthread_join(ctids[i], NULL);
    }

	/* Join results of all threads */
    count_info->encountered_error = pdata.encountered_error;
    for (int i = 0; i < opt.num_threads; i++) {
        count_info->total_lines += tdata[i].line_result;
        count_info->total_chars += tdata[i].char_result;
        count_info->total_words += tdata[i].word_result;
    }

    /* Cleanup */
    sem_unlink("empty_sem");
    for (int i = 0; i < opt.num_threads; i++) {
        char sem_name[20];
        sprintf(sem_name, "full_sem_%d", i);
        sem_unlink(sem_name);
    }
    free(full);
    for (int i = 0; i < opt.num_threads; i++) {
        free(data[i]);
    }
    free(data);
    free(ctids);
    free(tdata);
}


static void *
producer(void *arg)
{
    /* Extract arguments and initialize variables */
	producer_data *pdata = (producer_data *)arg;
    int consumer_id = -1;

	/* Open file and check for errors */
	FILE *file = fopen(pdata->input_file, "r");
	if (file == NULL) {
        /* Couldn't open file, so kill consumers */
        kill_consumers(pdata, full);

        /* Signal to main thread that error was encountered */
        pdata->encountered_error = true;

        /* Print error message to stdout */
        char error_output[1000];
        snprintf(error_output, 1000, "mwc: %s: open", pdata->input_file);
        perror(error_output);

        /* Return and kill producer */
        return NULL;
    }

	/* Keep reading from file, and passing to consumers */
    bool next_carry = false;
	while(true) {
		/* Find a consumer which is not busy */
		do {
        	consumer_id = (consumer_id+1) % pdata->num_threads; // Circular queue for index
		} while(pdata->tdata[consumer_id].busy);

		/* Semaphore to wait for consumers to empty their buffer */
        sem_wait(empty);

		/* BEGIN CRITICAL VALUES */
		size_t res = fread(data[consumer_id], sizeof(char), BUFFER_SIZE, file);
		pdata->tdata[consumer_id].num_elements = res;
		pdata->tdata[consumer_id].busy = true;
        if(pdata->tdata[consumer_id].count_words) {
            pdata->tdata[consumer_id].carry = next_carry;
            next_carry = !isspace(data[consumer_id][res-1]);
        }
        

		/* If end of file, finish producing */
		if(feof(file)) {
            fclose(file);
			kill_consumers(pdata, full);
			break;
		}

		/* END OF CRITICAL VALUES, signal the corresponding consumer */
		sem_post(full[consumer_id]);
    }

    return NULL;
}


static void
count_buffer(char *buffer, consumer_data *tdata)
{
    bool in_word = false;
    for(unsigned long i = 0; i < tdata->num_elements; i++) {
        if(buffer[i] == '\n') {
            tdata->line_result++;
        }
        if(tdata->count_words) {
            if(isspace(buffer[i])) { in_word = false; }
            else if(!in_word) { tdata->word_result++; in_word = true; }
        }
    }
    if(tdata->count_words && tdata->num_elements && tdata->carry && !isspace(buffer[0])) {
        tdata->word_result--;
    }
}


static void *
consumer(void *arg)
{
    /* Extract struct from argument */
	consumer_data *tdata = (consumer_data *)arg;
    int consumer_id = tdata->consumer_id;

	while(true) {
		/* Semaphore to wait for buffer to be full */
		sem_wait(full[consumer_id]);

		/* Start processing data from producer*/
        count_buffer(data[consumer_id], tdata);

		/* Reset buffer and set consumer as not busy */
		tdata->num_elements = 0;
		tdata->busy = false;
		sem_post(empty);

		/* If not alive, break */
		if(!tdata->isAlive) { break; }
	}

	return NULL;
}


/*================================
|  Helper Functions              |
================================*/

static int
get_num_threads(struct mwc_args_info args_info)
{
    int thread_max = max_user_threads();
    int num_threads;

    if(args_info.threads_given) {
        num_threads = MIN2(thread_max, args_info.threads_arg);
    } else {
        /* use maximum of concurrent threads */
        int proc_cores, proc_cores_conf;
        if (num_proc_cores(&proc_cores, &proc_cores_conf)) {
            num_threads = MIN2(thread_max, proc_cores_conf);
        } else {
            printf("WARNING: Could not determine number of available processor cores!\n"
                   "Defaulting to serial computation");
            num_threads = 1;
        }
    }
    num_threads = MAX2(1, num_threads);

    return num_threads;
}


static char **
collect_unnamed_options(struct mwc_args_info *ggostruct, int *num_files)
{
    char  **input_files = NULL;

    *num_files = 0;

    /* collect all unnamed options */
    if (ggostruct->inputs_num > 0) {
        input_files = (char **)realloc(input_files, sizeof(char *) * ggostruct->inputs_num);
        for (unsigned int i = 0; i < ggostruct->inputs_num; i++) {
            input_files[(*num_files)++] = strdup(ggostruct->inputs[i]);
        }
    }

  return input_files;
}


static void 
kill_consumers(producer_data *pdata, sem_t **full)
{
	for(int i=0; i<pdata->num_threads; i++) {
		pdata->tdata[i].isAlive = false; /* signal thread as not alive */
		sem_post(full[i]);
	}
}
