#include <stdio.h>
#include <stdlib.h>

int add(int a, int b) {
    return a + b;
}

int mult(int a, int b) {
    return a * b;
}

int sub(int a, int b) {
    return a - b;
}

int apply(int a, int b, int (*op)(int, int)) {
    return op(a, b);
}

typedef int (*binop)(int, int);

binop select_op(char c) {
    switch (c) {
    case '+':
        return add;
    case '-':
        return sub;
    case '*':
        return mult;
    default:
        return NULL;
    }
}

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "usage: %s operator int int\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* Remember to check for NULL! */
    binop fp = select_op(argv[1][0]);
    if (fp == NULL) {
        fprintf(stderr, "%s invalid operator\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    /* atoi ok for "throw-away" code */
    int num1 = atoi(argv[2]);
    int num2 = atoi(argv[3]);
    int result = fp(num1, num2);
    printf("Result of %s operation on %d and %d is %d\n", argv[1], num1,
           num2, result);


    return EXIT_SUCCESS;
}


