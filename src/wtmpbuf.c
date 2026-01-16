/* wtmpbuf.c */
/* Contains the implementations of the functions used in my_last.c */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include "wtmpbuf.h"


int
init_wtmp(const char *filename, struct utmpx **utbuf, int nrecs){
    /* Check inputs */
    if (filename == NULL || utbuf == NULL || nrecs <= 0) {
        errno = EINVAL;
        return -1;
    }

    *utbuf = NULL; /* for failures, just in case */
    int fd; /*file descriptor */
    if ((fd = open(filename, O_RDONLY)) == -1)
        return -1;

    /* Check if nrecs is an unreasonable size */
    size_t utsize = sizeof(struct utmpx);
    if ((size_t)nrecs > (SIZE_MAX / utsize)) {
        errno = ENOMEM;
        close(fd);
        return -1;
    }

    size_t bytes = (size_t)nrecs * utsize;
    *utbuf = malloc(bytes);
    if (*utbuf == NULL) { /* malloc failed */
        close(fd); /* close open file descriptor */
        return -1;
    }

    /* Successful open call and allocation of *utbuf */
    return fd;
}


struct utmpx*
get_next_utrec(struct utmpx *utbuf, int *next_ut) {
    if (utbuf == NULL || next_ut == NULL)
        return NULL;

    if (*next_ut < 0)
        return NULL;
    else
        return &utbuf[(*next_ut)--];
}


void
wtmp_finalize(int fd_wtmp, struct utmpx **utbuf){
    if (utbuf != NULL && *utbuf != NULL) {
        free(*utbuf);
        *utbuf = NULL;
    }
    if (fd_wtmp >= 0)
        close(fd_wtmp);
}
