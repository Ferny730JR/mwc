/** @file mwc_cmdl.h
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.23
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt */

#ifndef MWC_CMDL_H
#define MWC_CMDL_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef MWC_CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define MWC_CMDLINE_PARSER_PACKAGE "mwc - Multithreaded Word Count"
#endif

#ifndef MWC_CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define MWC_CMDLINE_PARSER_PACKAGE_NAME "mwc - Multithreaded Word Count"
#endif

#ifndef MWC_CMDLINE_PARSER_VERSION
/** @brief the program version */
#define MWC_CMDLINE_PARSER_VERSION "1.0"
#endif

/** @brief Where the command line options are stored */
struct mwc_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  int characters_flag;	/**< @brief Count the number of bytes in each input file, and write it to standard output. (default=off).  */
  const char *characters_help; /**< @brief Count the number of bytes in each input file, and write it to standard output. help description.  */
  int lines_flag;	/**< @brief Count the number of lines in each input file, and write it to standard output. (default=off).  */
  const char *lines_help; /**< @brief Count the number of lines in each input file, and write it to standard output. help description.  */
  int words_flag;	/**< @brief Count the number of words in each input file, and write it to standard output. (default=off).  */
  const char *words_help; /**< @brief Count the number of words in each input file, and write it to standard output. help description.  */
  int threads_arg;	/**< @brief Specify the number of threads to use. (default='0').  */
  char * threads_orig;	/**< @brief Specify the number of threads to use. original value given at command line.  */
  const char *threads_help; /**< @brief Specify the number of threads to use. help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int characters_given ;	/**< @brief Whether characters was given.  */
  unsigned int lines_given ;	/**< @brief Whether lines was given.  */
  unsigned int words_given ;	/**< @brief Whether words was given.  */
  unsigned int threads_given ;	/**< @brief Whether threads was given.  */

  char **inputs ; /**< @brief unnamed options (options without names) */
  unsigned inputs_num ; /**< @brief unnamed options number */
} ;

/** @brief The additional parameters to pass to parser functions */
struct mwc_cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure mwc_args_info (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure mwc_args_info (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *mwc_args_info_purpose;
/** @brief the usage string of the program */
extern const char *mwc_args_info_usage;
/** @brief the description string of the program */
extern const char *mwc_args_info_description;
/** @brief all the lines making the help output */
extern const char *mwc_args_info_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int mwc_cmdline_parser (int argc, char **argv,
  struct mwc_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use mwc_cmdline_parser_ext() instead
 */
int mwc_cmdline_parser2 (int argc, char **argv,
  struct mwc_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int mwc_cmdline_parser_ext (int argc, char **argv,
  struct mwc_args_info *args_info,
  struct mwc_cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int mwc_cmdline_parser_dump(FILE *outfile,
  struct mwc_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int mwc_cmdline_parser_file_save(const char *filename,
  struct mwc_args_info *args_info);

/**
 * Print the help
 */
void mwc_cmdline_parser_print_help(void);
/**
 * Print the version
 */
void mwc_cmdline_parser_print_version(void);

/**
 * Initializes all the fields a mwc_cmdline_parser_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void mwc_cmdline_parser_params_init(struct mwc_cmdline_parser_params *params);

/**
 * Allocates dynamically a mwc_cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized mwc_cmdline_parser_params structure
 */
struct mwc_cmdline_parser_params *mwc_cmdline_parser_params_create(void);

/**
 * Initializes the passed mwc_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void mwc_cmdline_parser_init (struct mwc_args_info *args_info);
/**
 * Deallocates the string fields of the mwc_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void mwc_cmdline_parser_free (struct mwc_args_info *args_info);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int mwc_cmdline_parser_required (struct mwc_args_info *args_info,
  const char *prog_name);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MWC_CMDL_H */
