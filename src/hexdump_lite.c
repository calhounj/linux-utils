#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define BYTES_PER_LINE 16

/* Print a brief usage message */
static void
print_usage(const char *progname)
{
    fprintf(stderr, "usage: %s FILE\n", progname);
    fprintf(stderr, "A minimal hexdump-style viewer.\n");
}

/* Print a single line of hexdump output.
 *
 * offset: byte offset from start of file
 * buf:    pointer to the bytes for this line
 * count:  how many bytes are valid in buf (1..BYTES_PER_LINE)
 */
static void
print_line(unsigned long offset, const unsigned char *buf, size_t count)
{
    /* Offset column */
    printf("%08lx: ", offset);

    /* Hex bytes column */
    for (size_t i = 0; i < BYTES_PER_LINE; i++) {
        if (i < count) {
            printf("%02x ", buf[i]);
        } else {
            /* pad if line is shorter than BYTES_PER_LINE */
            printf("   ");
        }

        /* Extra space in the middle for readability (8 + 8 grouping) */
        if (i == 7) {
            putchar(' ');
        }
    }

    putchar(' ');

    /* ASCII column */
    for (size_t i = 0; i < count; i++) {
        unsigned char c = buf[i];
        if (isprint(c)) {
            putchar(c);
        } else {
            putchar('.');
        }
    }

    putchar('\n');
}

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s: cannot open '%s': %s\n",
                argv[0], filename, strerror(errno));
        return EXIT_FAILURE;
    }

    unsigned char buffer[BYTES_PER_LINE * 4]; /* read a few lines at a time */
    ssize_t nread;
    unsigned long offset = 0;

    for (;;) {
        nread = read(fd, buffer, sizeof buffer);
        if (nread == -1) {
            if (errno == EINTR) {
                /* interrupted by signal; retry */
                continue;
            }
            fprintf(stderr, "%s: error reading '%s': %s\n",
                    argv[0], filename, strerror(errno));
            close(fd);
            return EXIT_FAILURE;
        }

        if (nread == 0) {
            /* EOF */
            break;
        }

        /* Process what we just read, BYTES_PER_LINE bytes per printed line */
        ssize_t processed = 0;
        while (processed < nread) {
            size_t line_count = (size_t)(nread - processed);
            if (line_count > BYTES_PER_LINE) {
                line_count = BYTES_PER_LINE;
            }

            print_line(offset, buffer + processed, line_count);

            processed += (ssize_t)line_count;
            offset   += (unsigned long)line_count;
        }
    }

    if (close(fd) == -1) {
        fprintf(stderr, "%s: error closing '%s': %s\n",
                argv[0], filename, strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

