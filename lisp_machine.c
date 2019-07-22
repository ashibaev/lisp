#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "runner.h"
#include "error_handling.h"


int32_t print_usage(char * name) {
    printf("Usage: %s <path_to_file>\n", name);
    return 1;
}

int32_t main(int32_t argc, char ** argv) {
    if (argc != 2)
        return print_usage(argv[0]);
    FILE* input;
    int rv;

    CHECK(input = fopen(argv[1], "r"), "open", argv[1], rv, 1, exit);

    rv = run_lisp(input, stdout);

    fclose(input);
    return rv;
exit:
    return rv;
}
