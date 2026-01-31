#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

struct ts {
    int64_t sec;
    uint32_t nsec;
};

/* Time selector */
enum time_kind {
    TIME_ATIME,
    TIME_BTIME,
    TIME_CTIME,
    TIME_MTIME,
};

int
compare_ts(struct ts a, struct ts b) {
    /* Compare seconds */
    if (a.sec > b.sec)
        return 1;
    if (a.sec < b.sec)
        return 0;

    /* Files created in the same second. Now compare nanoseconds */
    /* Result is boolean, 1 or 0, determining which file is newer */
    return a.nsec >= b.nsec;
}

/* get_ts() will call statx and use the flag STATX_MTIME for now.
 * Future improvements will allow flags to be flexible. Return 1 on
 * success and struct ts will be correct. Always have to check
 * that statx actually does what it's supposed to do. Return -1 on
 * failure. */
int
get_ts(const char *pathname, enum time_kind k, struct ts *sx) {
    /* Validate arguments */
    if (pathname == NULL || sx == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Fill correct mask */
    unsigned int mask;
    switch (k) {
    case TIME_ATIME:
        mask = STATX_ATIME;
        break;
    case TIME_BTIME:
        mask = STATX_BTIME;
        break;
    case TIME_CTIME:
        mask = STATX_CTIME;
        break;
    case TIME_MTIME:
        mask = STATX_MTIME;
        break;
    default:
        errno = EINVAL;
        return -1;
    }

    /* Fill a statx structure */
    struct statx out_ts = {0}; /* Just incase I look at it sometime */
    if (statx(AT_FDCWD, pathname, AT_SYMLINK_NOFOLLOW, mask, &out_ts) == -1)
        return -1; /* errno should be set */

    /* Successful statx call, check if wanted time is in the structure */
    if (out_ts.stx_mask & mask) {
        switch (k) {
        case TIME_ATIME:
            sx->sec = out_ts.stx_atime.tv_sec;
            sx->nsec = out_ts.stx_atime.tv_nsec;
            break;
        case TIME_BTIME:
            sx->sec = out_ts.stx_btime.tv_sec;
            sx->nsec = out_ts.stx_btime.tv_nsec;
            break;
        case TIME_CTIME:
            sx->sec = out_ts.stx_ctime.tv_sec;
            sx->nsec = out_ts.stx_ctime.tv_nsec;
            break;
        case TIME_MTIME:
            sx->sec = out_ts.stx_mtime.tv_sec;
            sx->nsec = out_ts.stx_mtime.tv_nsec;
            break;
        }
        return 1;
    }
    else {
        errno = ENODATA;
        return -1;
    }
}

int
newer_by(const char *pathname1, const char *pathname2, enum time_kind k) {
    /* Validate input */
    if (pathname1 == NULL || pathname2 == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Create structs */
    struct ts ts1, ts2;

    /* Convert first pathname to struct ts1 */
    if (get_ts(pathname1, k, &ts1) == -1)
        return -1;

    /* Convert second pathname to struct ts2 */
    if (get_ts(pathname2, k, &ts2) == -1)
        return -1;

    /* Successful ts creation */
    int result = compare_ts(ts1, ts2);
    return result;
}

int
newer(const char *pathname1, const char *pathname2) {
    /* Validate input */
    if (pathname1 == NULL || pathname2 == NULL) {
        errno = EINVAL;
        return -1;
    }
    else
        return newer_by(pathname1, pathname2, TIME_MTIME);
}

int
main(int argc, char *argv[]) {
    /* Usage: Only allow one option for now */
    /* Option parsing */
    char options[] = "abcm";
    int ch;
    enum time_kind k;
    int result;

    /* If there are no option supplied, we need default behavior */
    ch = getopt(argc, argv, options);
    /* proper usage check */
    if (argc - optind != 2) {
        fprintf(stderr, "usage: %s [option] pathname1 pathname2\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *p1 = argv[optind];
    const char *p2 = argv[optind + 1];
    if (ch == -1) {

        /* Set errno, newer or get_ts will modify if error */
        errno = 0;
        result = newer(p1, p2);
        /* newer failed, errno should be set */
        if (result < 0) {
            fprintf(stderr, "newer() failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    else {
        switch (ch) {
        case 'a':
            k = TIME_ATIME;
            break;
        case 'b':
            k = TIME_BTIME;
            break;
        case 'c':
            k = TIME_CTIME;
            break;
        case 'm':
            k = TIME_MTIME;
            break;
        default:
            fprintf(stderr, "Invalid option %c\n", ch);
            exit(EXIT_FAILURE);
        }
        /* Successful option parsing and time_kind k is set */
        /* But I need to check options one more time to enforce
         * my current 'only one option' rule */
        if ((ch = getopt(argc, argv, options)) != -1) {
            fprintf(stderr, "usage: too many options\n");
            exit(EXIT_FAILURE);
        }
        errno = 0;
        result = newer_by(p1, p2, k);
        if (result < 0) {
            fprintf(stderr, "newer_by failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }


    /* Success */
    if(result == 1)
        printf("%s is newer\n", argv[optind]);
    else if (result == 0)
        printf("%s is newer\n", argv[optind + 1]);
    else {
        fprintf(stderr, "Something failed\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
