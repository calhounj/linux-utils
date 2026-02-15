/* This program will help me practice using the functions
 * telldir and seekdir in the Directory API. The program
 * will print all directory entries that are directories
 * before all other files.
 *
 * Each time the program reads an entry in the given
 * directory stream, it checks whether or not it is a
 * directory. If it is not a directory, it saves it in
 * a list to print it later, and if it is a directory it
 * will print it immediately.*/
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#define TRUE 1
#define FALSE 0

#define LIST_DIRS_FIRST 1
#define SHOW_ALL 2

void
usage(const char *program) {
    fprintf(stderr, "usage: %s [options ...] file1 file2 ...\n", program);
    exit(EXIT_FAILURE);
}

/* Returns true if *direntp represents a directory, and false otherwise */
int
isdir(const struct dirent *direntp) {
#ifdef _DIRENT_HAVE_D_TYPE          /* We have the d_type member */
    return (direntp->d_type == DT_DIR);
#else
    struct stat statbuf;
    if (stat(direntp->d_name, &statbuf) == -1)
        return FALSE;
    else
        return (S_ISDIR(statbuf.st_mode));
#endif
}

/* Linked list node that stores an offset returned by telldir() */
typedef struct listnode {
    long pos;
    struct listnode *next;
} poslist;

/* save(p, &pos_listptr) saves the position p onto the end of the list pointed
 * to by pos_listptr. Because the list head might be changed, its address
 * is passed, not its value */
void
save(long pos, poslist **list) {
    poslist *pos_node_ptr;
    static poslist *last = NULL;

    /* Allocate new list node */
    errno = 0;
    pos_node_ptr = (poslist*) malloc(sizeof(poslist));
    if (pos_node_ptr == NULL)
        perror("malloc");

    /* Copy position into the new node */
    pos_node_ptr->pos = pos;

    /* Insert node into the end of the list */
    if (*list == NULL) {
        *list = pos_node_ptr;
    }
    else {
        last->next = pos_node_ptr; /* last will already be saved */
    }

    last = pos_node_ptr;
    pos_node_ptr->next = NULL;
}

/*printlist(dirp, pos_list) prints the filename whose offsets were saved into
 * pos_list. */
void
printlist(DIR *dirp, poslist *list) {
    poslist *node = list;

    while (node != NULL) {
        seekdir(dirp, node->pos);   /* Already checked for errors in main */
        printf("%s\n", readdir(dirp)->d_name);
        node = node->next;
    }
}

/*eraselist(&list) erases the list pointed to by list.*/
void
eraselist(poslist **list) {
    poslist *ptr = *list;
    poslist *next;

    while (ptr != NULL) {
        next = ptr->next;
        free(ptr);
        ptr = next;
    }

    *list = NULL; /* Take care of dangling pointer */
}

void
listdir(DIR *dirp, int flags) {
    struct dirent *entry;
    long int pos;
    poslist *saved_positions = NULL;

    while (1) {
        pos = telldir(dirp);    /* save current position */
        errno = 0;              /* try to read entry */
        if (NULL == (entry = readdir(dirp)) && errno != 0)
            perror("readdir");
        else if (entry == NULL) /* End of directory stream */
            break;
        else {
            if ((flags & LIST_DIRS_FIRST) && !isdir(entry)) {
                if (flags & SHOW_ALL) {
                    save(pos, &saved_positions);
                    continue;
                }
                else if (entry->d_name[0] != '.') {
                    save(pos, &saved_positions);
                    continue;
                }
            }
            if (flags & SHOW_ALL) {
                printf("%s/\n", entry->d_name);
            }
            else {
                if (entry->d_name[0] != '.')
                    printf("%s/\n", entry->d_name);
            }
        }
    }
    if (flags & LIST_DIRS_FIRST)
        printlist(dirp, saved_positions);
    eraselist(&saved_positions);
}

int
main(int argc, char *argv[]) {
    DIR *dirp;
    int i;
    char *options = ":a";
    int show_all = 0;

    while (1) {
        int ch = getopt(argc, argv, options);
        if (ch == -1)
            break;
        switch (ch) {
        case 'a':
            show_all = SHOW_ALL;
            break;
        case ':':
            fprintf(stderr, "Missing option argument\n");
            usage(argv[0]);
            break;
        default:
            usage(argv[0]);
            break;
        }
    }

    int ls_flags = (LIST_DIRS_FIRST | show_all);

    if (argc == optind) {        /* Use current directory */
        errno = 0;
        dirp = opendir(".");
        if (dirp == NULL)
            perror("opendir");
        else
            listdir(dirp, ls_flags);
    }
    else {
        for (i = optind; i < argc; i++) {
            errno = 0;
            if ((dirp = opendir(argv[i])) == NULL) {
                if (errno == ENOTDIR)
                    printf("%s\n", argv[i]);
                else
                    fprintf(stderr, "%s %s\n", argv[i], strerror(errno));
            }
            else {
                printf("\n%s:\n", argv[i]);
                listdir(dirp, ls_flags);
                closedir(dirp);
            }
        }
    }
    return 0;
}

