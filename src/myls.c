#define _POSIX_C_SOURCE 200809L

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>


#define TRUE 1
#define FALSE 0

static void
fatal_error(int errnum, const char *msg) {
    fprintf(stderr, "fatal error: %s:%s\n", msg, strerror(errnum));
    exit(EXIT_FAILURE);
}

static void
error_message(int errnum, const char *name) {
    fprintf(stderr, "opendir error: %s: %s", name, strerror(errnum));
}

/* Comparator function for qsort */
static int
cmp_strcoll(const void *a, const void *b)
{
    const char * const *sa = a;
    const char * const *sb = b;
    return strcoll(*sa, *sb);
}


/* listdir(dirp, flag) is from Weiss. This function prints the filenames
 * in the directory stream dirp, one per line, including . and .., in the
 * order the stream delivers them. The parameter flags will be used for
 * future extentions.*/
void
listdir(DIR *dirp, int flags) {
    struct dirent *direntp;     /* Pointer to a directory entry structure */
    int done = FALSE;           /* Flag to control loop execution */
    const char *name;           /* Name of directory or file */

    while (!done) {
        errno = 0;
        direntp = readdir(dirp);
        if (direntp == NULL && errno != 0)
            perror("readdir");
        else if (direntp == NULL)
            done = TRUE;
        else {
            name = direntp->d_name;
            if ((strcmp(name, ".") != 0) && (strcmp(name, "..") != 0)) {
                printf("    %s\n", name);
            }
        }
    }
    printf("\n");
}

int
main(int argc, char *argv[]) {
    DIR *dirp;
    int ls_flags = 0;

    if (setlocale(LC_COLLATE, "") == NULL) {
        fprintf(stderr, "setlocale failed\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 1) { /* No arguments, use current working directory */
        errno = 0;
        if ((dirp = opendir(".")) == NULL)
            fatal_error(errno, "opendir()");
        listdir(dirp, ls_flags);
    }
    else {          /* For each command line argument call readdir() */
        for (int i = 1; i < argc; i++) {
            errno = 0;
            if ((dirp = opendir(argv[i])) == NULL) {
                if (errno == ENOTDIR)   /* Argument not a directory */
                    printf("%s\n", argv[i]);
                else
                    error_message(errno, argv[i]); /* error and move on */
            }
            else {
                printf("%s:\n", argv[i]);
                listdir(dirp, ls_flags);
                closedir(dirp);
            }
        }
    }

    return EXIT_SUCCESS;
}

