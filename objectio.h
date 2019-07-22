#pragma once

#include <stdbool.h>

#include "stream_reader.h"
#include "state.h"
#include "object.h"
#include "id_types.h"

struct Object;

char* print_object_to_buffer(struct Object const * object, char* buffer);

struct Variables {
    char variables[MAX_TOKEN_LENGTH][MAX_ARGUMENT_COUNT];
    arguments_size_t argc;
};

#define PARSE_ARGS struct State state, struct TokenStreamReader* reader
#define PARSE_DEFINITION_ARGS PARSE_ARGS, struct Variables* variables, object_id_t object_id
#define PARSE_OBJECT_ARGS PARSE_ARGS, struct Variables const * variables, object_id_t output_object_id

struct State parse_object(PARSE_OBJECT_ARGS);

struct State parse_definition(PARSE_DEFINITION_ARGS);

#define FOR_EACH_EXPRESSION_TYPE(expr)      \
expr(INVALID_EXPRESSION) expr(OBJECT_EXPRESSION) expr(DEFINITION_EXPRESSION)

enum ExpressionType {
#define DEF(X) X,
    FOR_EACH_EXPRESSION_TYPE(DEF)
#undef DEF
};

struct Expression {
    enum ExpressionType type;
    object_id_t object_id;
};


struct Expression create_expression(enum ExpressionType type, object_id_t object_id);

struct Expression get_invalid_expression();


struct Expression parse_expression(struct TokenStreamReader* reader);

