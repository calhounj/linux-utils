#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/statvfs.h>

int
main(int argc, char *argv[]) {

    /* Create a buffer for statvfs */
    struct statvfs vfs;
    unsigned long bs = 0;
    if (statvfs("/", &vfs) == 0) {
        printf("%lx\n", vfs.f_fsid);
        if (vfs.f_frsize != 0)
            bs = vfs.f_frsize;
        else
            bs = vfs.f_bsize;
        printf("Total blocks: %lu\n", bs);
    }


    return 0;
}
