#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int
samefile(const char *pathname1, const char *pathname2) {
    /* Check pathname validity */
    if (pathname1 == NULL || pathname2 == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Create two statx structures and attempt to fill members */
    struct statx st1 = {0}, st2 = {0};
    unsigned int flags = STATX_INO;

    if (statx(AT_FDCWD, pathname1, AT_SYMLINK_NOFOLLOW, flags, &st1) == -1)
        return -1;

    if (statx(AT_FDCWD, pathname2, AT_SYMLINK_NOFOLLOW, flags, &st2) == -1)
        return -1;

    /* Check if pathnames point to same inode */
    if ((st1.stx_mask & STATX_INO) && (st2.stx_mask & STATX_INO)) {
        if (st1.stx_ino == st2.stx_ino &&
            st1.stx_dev_major == st2.stx_dev_major &&
            st1.stx_dev_minor == st2.stx_dev_minor)
            return 1;
        else
            return 0;
    }

    /* If we make it here, then st1 or st2 doesn't have stx_ino, so fail */
    errno = ENODATA;
    return -1;
}

int
main(int argc, char *argv[]) {

    /* Usage */
    if (argc != 3) {
        fprintf(stderr, "usage: %s file1 file2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    errno = 0;
    int result = samefile(argv[1], argv[2]);
    if (result == -1) {
        fprintf(stderr, "samefile failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (result)
        printf("1\n");
    else
        printf("0\n");

    return EXIT_SUCCESS;
}
