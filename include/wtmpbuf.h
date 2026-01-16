/* wtmpbuf.h */
#ifndef WTMP_BUF_H
#define WTMP_BUF_H

#include <utmpx.h>

/* Open the wtmp file specified by filename, optain a file descriptor, and
 * if successful, allocate storage for a buffer *utbuf of size
 * NRECS * UTSIZE (where NRECS is the number of records supplied by a
 * command line option and UTSIZE is sizeof(struct utmpx)).
 * Return the file descriptor if successful and -1 on failure. */
int init_wtmp(const char *filename, struct utmpx **utbuf, int nrecs);

/* Return a pointer to the next utmpx structure to process from the utbuf
 * buffer at index next_ut, decrementing next_ut. */
struct utmpx *get_next_utrec(struct utmpx *utbuf, int *next_ut);

/* Free all memory used by the buffer utbuf and close the file descriptor
 * fd_wtmp. */
void wtmp_finalize(int fd_wtmp, struct utmpx **utbuf);

#endif /* wtmpbuf.h */
