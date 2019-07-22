#include <stdint.h>

#include "runner.h"
#include "init.h"
#include "stream_reader.h"
#include "objectio.h"

int32_t run_lisp(FILE *input, FILE *output) {
    init_all();

    struct TokenStreamReader reader;
    init_stream_reader(&reader, get_next_token, seek_next_token, input);

    struct Expression expression;
    char buffer[10000];
    struct Object result;
    int32_t rv = 0;

    while (!feof(input)) {
        expression = parse_expression(&reader);
        switch (expression.type) {
            case OBJECT_EXPRESSION:
                get_value(&result, get_object_by_id(expression.object_id), 0, NULL);
                if (result.type == EXCEPTION_TYPE) {
                    rv = result.OBJECT_FIELD(EXCEPTION_TYPE);
                    goto clear;
                }
                print_object_to_buffer(&result, buffer);
                fprintf(output, "%s\n", buffer);
                break;
            case DEFINITION_EXPRESSION:
                fprintf(output, "function\n");
                break;
            default:
                return 1;
        }
    }

clear:
    clear_all();
    return rv ? rv : ferror(input);
}
