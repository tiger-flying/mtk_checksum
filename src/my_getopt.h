#ifndef _MY_GETOPT_H_
#define _MY_GETOPT_H_

#ifndef USE_SYS_GETOPT

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/*
 * I implement this function follow this manual: http://www.unix.com/man-page/POSIX/3posix/getopt/
 */

int my_getopt(int argc, char * const argv[], const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;

#ifdef __cplusplus
}
#endif // __cplusplus

#else // ifndef USE_SYS_GETOPT

#include <unistd.h>
#define my_getopt getopt

#endif // USE_SYS_GETOPT

#endif // _MY_GETOPT_H_
