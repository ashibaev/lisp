#include "object.h"
#include "function_pool.h"
#include "vector.h"


#define INIT_ARG(i)                                                                             \
do {                                                                                            \
    get_value(&call_args[i - 1], get_object_by_id(functionCall->arguments[i]), argc, args);     \
    if (call_args[i - 1].type == EXCEPTION_TYPE) {                                              \
        *result = call_args[i];                                                                 \
        return;                                                                                 \
    }                                                                                           \
    arg_ptrs[i - 1] = &call_args[i - 1];                                                        \
} while (0)



static void eval_function_call_value(struct Object *result,
                                     struct Object const *object,
                                     arguments_size_t argc,
                                     struct Object const **args)
{
    static function_id_t if_function_id = INVALID_FUNCTION_ID;
    static function_id_t cons_function_id = INVALID_FUNCTION_ID;

    if (if_function_id == INVALID_FUNCTION_ID)
        if_function_id = get_function("if")->id;
    if (cons_function_id == INVALID_FUNCTION_ID)
        cons_function_id = get_function("cons")->id;

    struct Object call_args[MAX_ARGUMENT_COUNT];
    struct Object* arg_ptrs[MAX_ARGUMENT_COUNT];

    struct FunctionCall* functionCall = get_function_call_by_id(object->OBJECT_FIELD(FUNCTION_CALL_TYPE));
    get_value(&call_args[0], get_object_by_id(functionCall->arguments[0]), argc, args);

    if (functionCall->argc == 1 && call_args[0].type != FUNCTION_TYPE) {
        *result = call_args[0];
        return;
    }

    if (call_args[0].type != FUNCTION_TYPE)
        EXIT_WITH_ERROR(RUNTIME_ERROR);

    struct Function* function = get_function_by_id(call_args[0].OBJECT_FIELD(FUNCTION_TYPE));

    if (function->id == if_function_id) {
        INIT_ARG(1);
        if (call_args[0].type != INTEGER_TYPE)
            EXIT_WITH_ERROR(TYPE_ERROR);

        size_t arg = call_args[0].OBJECT_FIELD(INTEGER_TYPE) ? 2 : 3;
        INIT_ARG(arg);
        *result = call_args[arg - 1];

        return;
    }

    for (size_t i = 1; i != functionCall->argc; ++i) {
        INIT_ARG(i);
    }

    if (function->id == cons_function_id) {
        object_id_t first = create_new_object();
        object_id_t second = create_new_object();

        *get_object_by_id(first) = call_args[0];
        *get_object_by_id(second) = call_args[1];

        call_args[0].id = first;
        call_args[1].id = second;
    }

    if (function->type == SIMPLE_FUNCTION) {
        function->call(result, (arguments_size_t) (functionCall->argc - 1), (const struct Object **) arg_ptrs );
        return;
    }

    get_value(result,
              get_object_by_id(function->object_id),
              (arguments_size_t) (functionCall->argc - 1),
              (const struct Object **) arg_ptrs);

}


void get_value(struct Object *result, struct Object const *object, arguments_size_t argc, struct Object const ** args) {
    switch (object->type) {
        case INTEGER_TYPE:
        case NULL_TYPE:
        case PAIR_TYPE:
        case EXCEPTION_TYPE:
        case FUNCTION_TYPE:
            *result = *object;
            return;
        case FUNCTION_ARGUMENT_TYPE:
            *result = *(args[object->OBJECT_FIELD(FUNCTION_ARGUMENT_TYPE)]);
            return;
        case FUNCTION_CALL_TYPE:
            eval_function_call_value(result, object, argc, args);
            return;
        default:
            EXIT_WITH_ERROR(RUNTIME_ERROR);
    }
}

static VECTOR_TYPE(Object)* objects = NULL;


object_id_t create_null_object() {
    return 0;
}


object_id_t create_new_object() {
    objects = VECTOR_FUNC(add_element, Object)(objects);
    struct Object* object = VECTOR_FUNC(mutable_back, Object)(objects);
    return object->id = (object_id_t) (objects->size - 1);
}

object_id_t create_new_integer(int32_t value) {
    struct Object* new_object = get_object_by_id(create_new_object());
    new_object->type = INTEGER_TYPE;
    new_object->OBJECT_FIELD(INTEGER_TYPE) = value;
    return new_object->id;
}

struct Object *get_object_by_id(object_id_t id) {
    return VECTOR_FUNC(get_mutable_elem, Object)(objects, id);
}

void init_objects_pool() {
    objects = VECTOR_FUNC(empty, Object)();
    struct Object* null = get_object_by_id(create_new_object());
    null->type = NULL_TYPE;
}

void clear_objects_pool() {
    VECTOR_FUNC(free, Object)(objects);
}

object_id_t create_function_call_object(function_call_id_t function_call_id) {
    struct Object* new_object = get_object_by_id(create_new_object());
    new_object->type = FUNCTION_CALL_TYPE;
    new_object->OBJECT_FIELD(FUNCTION_CALL_TYPE) = function_call_id;
    return new_object->id;
}

object_id_t create_function_argument(arguments_size_t position) {
    struct Object* new_object = get_object_by_id(create_new_object());
    new_object->type = FUNCTION_ARGUMENT_TYPE;
    new_object->OBJECT_FIELD(FUNCTION_ARGUMENT_TYPE) = position;
    return new_object->id;
}

object_id_t create_function_object(function_id_t function_id) {
    struct Object* new_object = get_object_by_id(create_new_object());
    new_object->type = FUNCTION_TYPE;
    new_object->OBJECT_FIELD(FUNCTION_TYPE) = function_id;
    return new_object->id;
}

object_id_t create_pair_object(object_id_t first, object_id_t second) {
    struct Object* new_object = get_object_by_id(create_new_object());
    new_object->type = PAIR_TYPE;
    new_object->OBJECT_FIELD(PAIR_TYPE).first = first;
    new_object->OBJECT_FIELD(PAIR_TYPE).second = second;
    return new_object->id;
}


