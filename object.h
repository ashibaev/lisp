#pragma once

#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include "id_types.h"

#define EXIT_WITH_ERROR(CODE)                       \
do {                                                \
    result->type = EXCEPTION_TYPE;                  \
    result->OBJECT_FIELD(EXCEPTION_TYPE) = CODE;    \
    return;                                         \
} while (0)
#define MAX_ARGUMENT_COUNT 10

#define FOR_EACH_OBJECT_TYPE(expr)                                          \
expr(NULL_TYPE) expr(INTEGER_TYPE) expr(PAIR_TYPE) expr(FUNCTION_TYPE)      \
expr(FUNCTION_CALL_TYPE) expr(FUNCTION_ARGUMENT_TYPE) expr(EXCEPTION_TYPE)

enum ObjectType {
#define DEFINE(X) X,
    FOR_EACH_OBJECT_TYPE(DEFINE)
#undef DEFINE
};

enum FunctionType {
    SIMPLE_FUNCTION,
    COMPOSITE_FUNCTION
};

struct Object;

typedef void (*Call)(struct Object* result, arguments_size_t argc, struct Object const ** args);

struct Function {
    union {
        Call call;
        object_id_t object_id;
    };
    object_id_t id;
    arguments_size_t arguments_amount;
    enum FunctionType type;
};

struct FunctionCall {
    object_id_t arguments[MAX_ARGUMENT_COUNT];
    arguments_size_t argc;
};

struct Pair {
    object_id_t first;
    object_id_t second;
};

enum ExceptionCode {
    TYPE_ERROR = 5,
    RUNTIME_ERROR = 6,
    DIVISION_BY_ZERO_ERROR = 7
};

#define OBJECT_FIELD(TYPE) TYPE ## _FIELD

struct Object {
    union {
        int32_t OBJECT_FIELD(INTEGER_TYPE);
        arguments_size_t OBJECT_FIELD(FUNCTION_ARGUMENT_TYPE);   // position
        function_id_t OBJECT_FIELD(FUNCTION_TYPE);
        function_call_id_t OBJECT_FIELD(FUNCTION_CALL_TYPE);
        struct Pair OBJECT_FIELD(PAIR_TYPE);
        enum ExceptionCode OBJECT_FIELD(EXCEPTION_TYPE);
        null_t OBJECT_FIELD(NULL_TYPE);
    };
    object_id_t id;
    enum ObjectType type : 16;
};



void get_value(struct Object* result, struct Object const * object, arguments_size_t argc, struct Object const ** args);

void init_objects_pool();
void clear_objects_pool();
object_id_t create_new_object();

object_id_t create_new_integer(int32_t value);
object_id_t create_null_object();
object_id_t create_function_call_object(function_call_id_t function_call_id);
object_id_t create_function_argument(arguments_size_t position);
object_id_t create_function_object(function_id_t function_id);
object_id_t create_pair_object(object_id_t first, object_id_t second);

struct Object* get_object_by_id(object_id_t);
