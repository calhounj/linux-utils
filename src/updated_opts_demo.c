#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("This is a function for testing getopt_long().\n");
        printf("Available options:\n");
        printf("--alpha\t--beta\t--calc (long int required)\t"
               "--gamma (long int optional)\n");
        return EXIT_SUCCESS;
    }

    static struct option long_opts[] = {
        {"alpha", no_argument, NULL, 'a'},
        {"beta" , no_argument, NULL, 'b'},
        {"calc" , required_argument, NULL, 'c'},
        {"gamma", optional_argument, NULL, 'g'},
        {0, 0, 0, 0}
    };

    int opt;
    long calc_val;
    long gamma_val;
    char *endptr;

    printf("Before parsing, optind=%d, "
           "argv[optind]=%s\n", optind, argv[optind]);

    while(-1 != (opt = getopt_long(argc, argv, ":abc:g::", long_opts, NULL))) {
        printf("After getopt_long call, opt = '%c' (%d), optind = %d\n",
               (opt > 0 && opt < 128 ? opt : '?'), opt, optind);

        switch(opt) {
        case 'a':
            printf("[case 'a'] alpha set and optind=%d\n", optind);
            break;
        case 'b':
            printf("[case 'b'] beta set, optind=%d\n", optind);
            break;
        case 'c':
            calc_val = strtol(optarg, &endptr, 10);
            printf("[case 'c'] calc value: %ld, optind=%d\n", calc_val, optind);
            break;
        case 'g':
            if (optarg != NULL) {
                gamma_val = strtol(optarg, &endptr, 10);
                printf("[case 'g'] gamma value: %ld, "
                       "optind=%d\n", gamma_val, optind);
            }else
                printf("[case 'g'] no argument provided for gamma; "
                       "using default behavior, optind=%d\n", optind);
            break;
        case ':':
            fprintf(stderr, "%s: option %c requires argument\n",
                    argv[0], optopt);
            exit(EXIT_FAILURE);
        case '?':
        default:
            /* This will only return an invalid short option */
            fprintf(stderr, "[case invalid] optind=%d\n", optind);
            break;
        }
    }

    printf("Finished parsing arguments, optind=%d\n", optind);

    /* See optind behavior */
    for (int i = optind; i < argc; i++) {
    printf("non-option argument: argv[%d]=%s\n", i, argv[i]);
    }


    return EXIT_SUCCESS;


}
