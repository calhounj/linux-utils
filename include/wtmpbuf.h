/* wtmpbuf.h */
#ifndef WTMP_BUF_H
#define WTMP_BUF_H

#include <utmpx.h>

/* This function returns a pointer to the next record in the wtmp
 * file in reverse order. Returns NULL when all records are read.
 * On error it also returns NULL and sets errno.*/
struct utmpx *next_wtmp_rec(int fd_utmp, struct utmpx *utbuf, int nrecs);

/* Open the wtmp file specified by filename, optain a file descriptor, and
 * if successful, allocate storage for a buffer *utbuf of size
 * NRECS * UTSIZE (where NRECS is the number of records supplied by a
 * command line option and UTSIZE is sizeof(struct utmpx)).
 * Return the file descriptor if successful and -1 on failure. */
int init_wtmp(const char *filename, struct utmpx **utbuf, int nrecs);

/* Return a pointer to the next utmpx structure to process from the utbuf
 * buffer at index next_ut, decrementing next_ut. */
struct utmpx *get_next_utrec(struct utmpx *utbuf, int *next_ut);

/* Try to read the next nrecs utmpx structures to process from fd_utmp
 * into the buffer utbuf starting at the beginning of the buffer.
 * Return the number actually read, or -1 if reading failed */
int load_buf(int fd_utmp, struct utmpx *utbuf, int nrecs);

/* Free all memory used by the buffer utbuf and close the file descriptor
 * fd_wtmp. */
void wtmp_finalize(int fd_wtmp, struct utmpx **utbuf);

#endif /* wtmpbuf.h */
