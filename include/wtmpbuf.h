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

/* Free all memory used by the buffer utbuf and close the file descriptor
 * fd_wtmp. */
void wtmp_finalize(int fd_wtmp, struct utmpx **utbuf);

#endif /* wtmpbuf.h */
