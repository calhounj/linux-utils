#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>       /* For WTMP_FILE */
#include <paths.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "wtmpbuf.h"

#define DEFAULT_NRECS 16

static void
usage(char *argv) {
    fprintf(stderr, "usage: %s [-x] [-b nrecs]\n", argv);
    exit(EXIT_FAILURE);
}

static void
die(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

/* get_long() -- on successful processing, returns 0 and stores the resulting
 * number in *value; otherwise, it returns -1 and puts a suitable message in
 * *msg.*/
static int
get_long(char *arg, long *value, char *msg) {

    char *endptr;
    long val;
    errno = 0;
    val = strtol(arg, &endptr, 10);

    if (errno == ERANGE) {
        if (msg != NULL)
            sprintf(msg, "%s\n", strerror(errno));
        return -1;
    }

    if (endptr == arg) {
        if (msg != NULL)
            sprintf(msg, "No digits in the command line argument\n");
        return -1;
    }

    if (*endptr != '\0') {
        if (msg != NULL)
            sprintf(msg, "Trailing characters follow the command "
                    "line argument: \"%s\"\n", endptr);
        return -1;
    }

    /* Successful proccessing of command line argument */
    *value = val;
    return 0;
}

/* get_int() returns 0 on successful processing and stores the resulting
 * value in *value. On error it returns -1 and a suitable message in msg
 * for error handling. */
static int
get_int(char *arg, int *value, char *msg) {

    long val;
    int result = get_long(arg, &val, msg);

    if (result == 0) {
        if ((val > 10000 || val < 1) && msg != NULL) {
            sprintf(msg, "Argument for number of records to read "
                    "is out of range\n");
            return -1;
        } else {
            *value = val;
            return 0;
        }
    } else return result; /* get_long failed */
}



int
main(int argc, char *argv[]) {

    int ch;
    char options[] = ":xb:";
    int opt_b = 0;      /* Must initialize to avoid classic c bug */
    char err_msg[256];         /* For passing error messages to main */

    struct config {
        int nrecs;
        int show_sys;
        const char *filename;
    };

    struct config my_options = {DEFAULT_NRECS, 0, WTMP_FILE};
    opterr = 0;
    while (1) {
        ch = getopt(argc, argv, options);
        if (-1 == ch)
            break;
        switch (ch) {
        case 'b':
            opt_b = get_int(optarg, &(my_options.nrecs), err_msg);
            break;
        case 'x':
            my_options.show_sys = 1;
            break;
        case ':':
            usage(argv[0]);
            break;
        case '?':
            usage(argv[0]);
            break;
        }
    }

    /* Check that opt_b processing was successful */
    if (opt_b == -1) die(err_msg); /*This is why we initialized opt_b */

    /* Initialize wtmp file and allocate memory for utbuf */
    struct utmpx *utbuf;
    int fd_utmp = init_wtmp(WTMP_FILE, &utbuf, my_options.nrecs);
    if (fd_utmp == -1) die("open wtmp");

    struct utmpx *ut;
    while (1) {
        errno = 0; /* Needs to be set on every iteration. Sticky errno! */
        ut = next_wtmp_rec(fd_utmp, utbuf, my_options.nrecs);
        if (ut == NULL)
            break;
        printf("Record time: %lld,"
               " Record type: %d\n", (long long)ut->ut_tv.tv_sec, ut->ut_type);
    }

    /* Check for errors */
    if (errno == 0) {
        wtmp_finalize(fd_utmp, &utbuf);
        printf("Done.\n");
    }
    else {
        fprintf(stderr, "%s\n", strerror(errno));
        wtmp_finalize(fd_utmp, &utbuf);
        exit(EXIT_FAILURE);
    }


    return 0;
}
