#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "mwc_cmdl.h"


struct options {
    bool provided_options;
    bool count_chars;
    bool count_lines;
    bool count_words;
};


void
init_default_options(struct options *opt)
{
    opt->provided_options = false;
    opt->count_chars = false;
    opt->count_lines = false;
    opt->count_words = false;
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

    toggle_options(&opt);

    return 0;
}
