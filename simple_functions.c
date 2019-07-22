#include <string.h>
#include "simple_functions.h"


#define CHECK_INTEGER_ARGS                                                              \
    if (args[0]->type != INTEGER_TYPE || args[1]->type != INTEGER_TYPE) {               \
        result->type = EXCEPTION_TYPE;                                                  \
        result->OBJECT_FIELD(EXCEPTION_TYPE) = TYPE_ERROR;                              \
        return;                                                                         \
    }

#define IMPLEMENT_INTEGER_FUNCTION(name, op, code)                                      \
DEFINE_FUNCTION(name) {                                                                 \
    CHECK_INTEGER_ARGS;                                                                 \
    code;                                                                               \
    result->type = INTEGER_TYPE;                                                        \
    result->OBJECT_FIELD(INTEGER_TYPE) =                                                \
        args[0]->OBJECT_FIELD(INTEGER_TYPE) op args[1]->OBJECT_FIELD(INTEGER_TYPE);     \
}

#define IMPLEMENT_SIMPLE_INTEGER_FUNCTION(name, op) IMPLEMENT_INTEGER_FUNCTION(name, op, (void) 0)
#define IMPLEMENT_INTEGER_FUNCTION_WITH_DIVISION(name, op)                              \
IMPLEMENT_INTEGER_FUNCTION(name, op,                                                    \
    if (args[1]->OBJECT_FIELD(INTEGER_TYPE) == 0) {                                     \
        result->type = EXCEPTION_TYPE;                                                  \
        result->OBJECT_FIELD(EXCEPTION_TYPE) = DIVISION_BY_ZERO_ERROR;                  \
        return;                                                                         \
    }                                                                                   \
)

IMPLEMENT_SIMPLE_INTEGER_FUNCTION(plus, +)
IMPLEMENT_SIMPLE_INTEGER_FUNCTION(minus, -)
IMPLEMENT_SIMPLE_INTEGER_FUNCTION(multiplies, *)
IMPLEMENT_SIMPLE_INTEGER_FUNCTION(less, <)
IMPLEMENT_SIMPLE_INTEGER_FUNCTION(greater, >)
IMPLEMENT_SIMPLE_INTEGER_FUNCTION(equal_to, ==)

IMPLEMENT_INTEGER_FUNCTION_WITH_DIVISION(divides, /)
IMPLEMENT_INTEGER_FUNCTION_WITH_DIVISION(modulus, %)

#undef CHECK_INTEGER_ARGS
#undef IMPLEMENT_SIMPLE_INTEGER_FUNCTION
#undef IMPLEMENT_INTEGER_FUNCTION

DEFINE_FUNCTION(cons) {
    result->type = PAIR_TYPE;
    result->OBJECT_FIELD(PAIR_TYPE).first = args[0]->id;
    result->OBJECT_FIELD(PAIR_TYPE).second = args[1]->id;
}

#define DEFINE_PAIR_GETTER(NAME, ELEMENT)                                                       \
    DEFINE_FUNCTION(NAME) {                                                                     \
        if (args[0]->type != PAIR_TYPE) {                                                       \
            result->type = EXCEPTION_TYPE;                                                      \
            result->OBJECT_FIELD(EXCEPTION_TYPE) = TYPE_ERROR;                                  \
            return;                                                                             \
        }                                                                                       \
        *result = *get_object_by_id(args[0]->OBJECT_FIELD(PAIR_TYPE).ELEMENT);                  \
    }

DEFINE_PAIR_GETTER(car, first)
DEFINE_PAIR_GETTER(cdr, second)

DEFINE_FUNCTION(if) {
    // args[0] - cond
    // args[1] - cond == true
    // args[2] - cond == false
    if (argc != 3) {
        EXIT_WITH_ERROR(RUNTIME_ERROR);
    }
    if (args[0]->type != INTEGER_TYPE) {
        result->type = EXCEPTION_TYPE;
        result->OBJECT_FIELD(EXCEPTION_TYPE) = TYPE_ERROR;
        return;
    }

    if (args[0]->OBJECT_FIELD(INTEGER_TYPE)) {
        *result = *args[1];
    } else {
        *result = *args[2];
    }


}
