#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <semaphore.h>

#include "mwc_cmdl.h"
#include "parallel_helpers.h"

static void *
producer(void *args);

static void *
consumer(void *args);

struct options {
    bool provided_options;
    bool count_chars;
    bool count_lines;
    bool count_words;
    unsigned int num_threads;
};


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
print_options(struct options opt)
{
    printf("Max user threads: %u\n",opt.num_threads);
}


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

    toggle_options(&opt);

    /*==========================================
    |  Begin Main Computations                 |
    ==========================================*/

    print_options(opt);
    return 0;
}

static void *
producer(void *args)
{
    return NULL;
}


static void *
consumer(void *args)
{
    return NULL;
}
