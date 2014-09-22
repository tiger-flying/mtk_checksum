#include "my_getopt.h"

#ifndef USE_SYS_GETOPT

#include <string.h>
#include <stdio.h>
#include <assert.h>

char *optarg = NULL;
int optind = 1;

/* whether to print error message to stderr. getopt will print error message
   if opterr is 1 and the first character is not ':'. */
int opterr = 1;

int optopt;

/*
 * With the given optstring and opt, find out the type of the opt.
 * @return
 *    -1 the @opt is not in the @optstring
 *    0  the @opt is an option that has no argument
 *    1  the @opt is an option that has argument
 */
static int get_type(const char *optstring, int opt)
{
  const char *p = optstring;
  while (*p != '\0' && *p != opt) {
    p++;
  }

  if (*p == '\0')
    return -1;
  else if (*(p + 1) == ':')
    return 1;

  return 0;
}

int my_getopt(int argc, char * const argv[], const char * optstring)
{
  assert(argv != NULL);

  const int err_or_end = -1;

  if (optstring == NULL || optstring[0] == '\0')
    return err_or_end;

  // when and only when opterr equals to 1 and optstring[0] is not ':' that we will print error messages
  const int print_err = (optstring[0] != ':' && opterr == 1)? 1 : 0;

  const int invalid_option_flag = '?';
  const int missing_argument_flag = (optstring[0] == ':')? ':' : invalid_option_flag;

  static int idx_in_arg = 1;

  if (argv[optind] == NULL || optind >= argc)
    return err_or_end;
  else if (argv[optind][0] == '-') {
    optopt = argv[optind][idx_in_arg];
    int type = get_type(optstring, optopt);
    int ret = optopt;


    if (0 == strcmp("-", argv[optind])) {
      // argv[optind] is "-", so we want to terminate the parsing
      // set to a non-exist type to indicate the state
      type = 2;
    } else if (0 == strcmp("--", argv[optind])) {
      // argv[optind] is "--"
      type = 3;
      optind++;
    } else if (argv[optind][++idx_in_arg] == '\0') {
      // no more options in argv[optind]
      idx_in_arg = 1;
      optind++;
    }

    switch (type) {
      case -1:
        ret = invalid_option_flag;
        if (print_err)
          fprintf(stderr, "%s: invalid option -- %c\n", argv[0], optopt);
        break;
      case 0:
        optarg = NULL;
        break;
      case 1: {
        if (optind >= argc) {
          ret = missing_argument_flag;
          if (print_err)
            fprintf(stderr, "%s: option requires an argument -- %c\n", argv[0], optopt);
          optarg = NULL;
        } else {
          optarg = (idx_in_arg == 1)? argv[optind] : (argv[optind] + idx_in_arg);
          idx_in_arg = 1;
        }
        optind++;
        break;
      }
      default:
        ret = err_or_end;
        break;
    }

    return ret;
  }

  return err_or_end;
}

#endif // USE_SYS_GETOPT
