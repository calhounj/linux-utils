//
// Created by john_c on 12/7/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>

/* -------------------------------------------------------------------------
   Program: textinfo
   Purpose: Demonstrate getopt() and getopt_long() by providing several
            file-inspection operations such as counting lines, words, chars,
            and converting to uppercase output.
   ------------------------------------------------------------------------- */

/* Flags set by command-line options */
static int opt_lines = 0;
static int opt_words = 0;
static int opt_chars = 0;
static int opt_uppercase = 0;

/* Print a usage/help message */
static void
print_usage(const char *progname)
{
    fprintf(stderr,
        "usage: %s [OPTIONS] file\n"
        "\n"
        "Options:\n"
        "  -l, --lines       Count lines in the file\n"
        "  -w, --words       Count words in the file\n"
        "  -c, --chars       Count characters in the file\n"
        "  -u, --uppercase   Print file contents in uppercase\n"
        "  -h, --help        Show this message\n"
        "\n"
        "If no -l/-w/-c options are provided, the program prints all three.\n"
        "If -u/--uppercase is given, counting is skipped and uppercase text\n"
        "is printed instead.\n",
        progname);
}

/* Count lines, words, and chars in a file */
static void
do_counts(FILE *fp)
{
    /*
       Suggested approach:
         - Use fgetc() in a loop.
         - Maintain counters: line_count, word_count, char_count.
         - Track transitions between whitespace and non-whitespace to detect words.
         - Print results at the end. */
    int lines = 0, words = 0, chars = 0;
    int in_word = 0;
    int ch;

    while ((ch = fgetc(fp)) != EOF) {
        chars++;

        if (ch == '\n')
            lines++;

        if (isspace((unsigned char)ch)) {
            in_word = 0;
        } else {
            if (!in_word) {
                words++;
                in_word = 1;
            }
        }
    }

    if (opt_lines) printf("Lines: %d\n", lines);
    if (opt_words) printf("Words: %d\n", words);
    if (opt_chars) printf("Chars: %d\n", chars);

}

/* Print file contents in uppercase */
static void
do_uppercase(FILE *fp)
{
    int ch;

    while ((ch = fgetc(fp)) != EOF) {
        if (isalpha((unsigned char)ch)) {
            ch = toupper((unsigned char)ch);
        }
        putchar(ch);
    }
}

/* ------------------------------------------------------------------------- */

int
main(int argc, char *argv[])
{
    int opt;

    /* Define long options */
    static struct option long_opts[] = {
        {"lines",     no_argument,       0, 'l'},
        {"words",     no_argument,       0, 'w'},
        {"chars",     no_argument,       0, 'c'},
        {"uppercase", no_argument,       0, 'u'},
        {"help",      no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    /* getopt loop */
    while ((opt = getopt_long(argc, argv, "lwcuh", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'l':
                opt_lines = 1;
                break;
            case 'w':
                opt_words = 1;
                break;
            case 'c':
                opt_chars = 1;
                break;
            case 'u':
                opt_uppercase = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case '?':   /* Unknown option */
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    /* Remaining arguments should contain the filename */
    if (optind >= argc) {
        fprintf(stderr, "error: missing filename\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[optind];

    /* If none of -l/-w/-c was selected, default to all three */
    if (!opt_uppercase && !(opt_lines || opt_words || opt_chars)) {
        opt_lines = opt_words = opt_chars = 1;
    }

    /* Open the file */
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "error: cannot open '%s': %s\n",
                filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Dispatch based on options */
    if (opt_uppercase) {
        do_uppercase(fp);
    } else {
        do_counts(fp);
    }

    fclose(fp);
    return EXIT_SUCCESS;
}
