/* wtmpbuf.c */
/* Contains the implementations of the functions used in my_last.c */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include "wtmpbuf.h"

/* Initialize and allocate */
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

/* Get the next record */
struct utmpx*
get_next_utrec(struct utmpx *utbuf, int *next_ut) {
    if (utbuf == NULL || next_ut == NULL)
        return NULL;

    if (*next_ut < 0)
        return NULL;
    else
        return &utbuf[(*next_ut)--];
}

/* Load the buffer with bytes */
int
load_buf(int fd_utmp, struct utmpx *utbuf, int nrecs) {

    if (utbuf == NULL)
        return -1;
    if (fd_utmp < 0 || nrecs <= 0) {
        errno = EINVAL;
        return -1;
    }

    size_t utsize = sizeof(struct utmpx);
    off_t chunk_bytes = (off_t)nrecs * (off_t)utsize;
    static off_t next_chunk_start;
    static off_t file_end;
    static int initialized = 0;
    static int done = 0;
    off_t bytes_available;
    off_t bytes_to_read;
    ssize_t bytes_read;
    size_t nrecs_read;

    /* Determine file size */
    /* Initial chunk start */
    if (!initialized) {
        file_end = lseek(fd_utmp, 0, SEEK_END);

        if (file_end == -1)
            return (int)file_end;

        if (file_end % utsize != 0) {
            fprintf(stderr, "File not a multiple of struct size");
            return -1;
        }

        if ((next_chunk_start = file_end - chunk_bytes) < 0)
            next_chunk_start = 0;
        initialized = 1;
    }

    /* return 0 records read if finished */
    if (done)
        return 0;

    /* Read into the buffer */
    if (lseek(fd_utmp, next_chunk_start, SEEK_SET) == -1)
        return -1; /* errno is set, main will handle */

    bytes_available = file_end - next_chunk_start;
    if (chunk_bytes <= bytes_available)
        bytes_to_read = chunk_bytes;
    else
        bytes_to_read = bytes_available;

    bytes_read = read(fd_utmp, utbuf, bytes_to_read);
    if (bytes_read < 0)
        return -1; /* errno is set, main will handle */
    if (bytes_read % utsize != 0) {
        errno = EIO;
        return -1;
    }

    /* Successful read of records */
    if (next_chunk_start == 0)
        done = 1;
    else if ((next_chunk_start -= chunk_bytes) < 0)
        next_chunk_start = 0; /* One more read call */

    nrecs_read = (size_t)bytes_read / utsize;
    return (int)nrecs_read;


}


/* Clean-up */
void
wtmp_finalize(int fd_wtmp, struct utmpx **utbuf){
    if (utbuf != NULL && *utbuf != NULL) {
        free(*utbuf);
        *utbuf = NULL;
    }
    if (fd_wtmp >= 0)
        close(fd_wtmp);
}
