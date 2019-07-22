#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "objectio.h"
#include "object.h"
#include "function_pool.h"
#include "state.h"

char* print_object_to_buffer(struct Object const * object, char* buffer) {
    switch (object->type) {
        case NULL_TYPE:
            return buffer + sprintf(buffer, "null");
        case INTEGER_TYPE:
            return buffer + sprintf(buffer, "%d", object->OBJECT_FIELD(INTEGER_TYPE));
        case PAIR_TYPE:
            buffer += sprintf(buffer, "(cons ");
            buffer = print_object_to_buffer(get_object_by_id(object->OBJECT_FIELD(PAIR_TYPE).first), buffer);
            buffer += sprintf(buffer, " ");
            buffer = print_object_to_buffer(get_object_by_id(object->OBJECT_FIELD(PAIR_TYPE).second), buffer);
            return buffer + sprintf(buffer, ")");
        default:
            return buffer;
    }
}


#define PARSE_TOKEN(TYPE) parse_token_ ## TYPE
#define DEFINE_PARSE_TOKEN(TYPE) struct State PARSE_TOKEN(TYPE)(PARSE_OBJECT_ARGS, struct Token* token)
#define CHECK_STATE(STATE) do {if (state.type == INVALID_STATE) return state;} while(0)

DEFINE_PARSE_TOKEN(Number) {
    struct Object* object = get_object_by_id(output_object_id);
    object->type = INTEGER_TYPE;
    object->OBJECT_FIELD(INTEGER_TYPE) = atoi(token->string);
    return state;
}

DEFINE_PARSE_TOKEN(Operation) {
    struct Object* object = get_object_by_id(output_object_id);
    object->type = FUNCTION_TYPE;
    object->OBJECT_FIELD(FUNCTION_TYPE) = get_function(token->string)->id;
    return state;
}

DEFINE_PARSE_TOKEN(Name) {
    struct Object* object = get_object_by_id(output_object_id);
    for (arguments_size_t i = 0; variables && i < variables->argc; ++i) {
        if (strcmp(variables->variables[i], token->string) == 0) {
            object->type = FUNCTION_ARGUMENT_TYPE;
            object->OBJECT_FIELD(FUNCTION_ARGUMENT_TYPE) = i;
            return state;
        }
    }
    struct Function* function = get_function(token->string);
    if (!function)
        return get_invalid_state();
    object->type = FUNCTION_TYPE;
    object->OBJECT_FIELD(FUNCTION_TYPE) = function->id;
    return state;
}

DEFINE_PARSE_TOKEN(OpeningBracket) {
    object_id_t object_id[MAX_ARGUMENT_COUNT + 1];
    for (arguments_size_t i = 0; i <= MAX_ARGUMENT_COUNT; ++i) {
        object_id[i] = create_new_object();
        struct State next_state = parse_object(state, reader, variables, object_id[i]);
        CHECK_STATE(next_state);
        if (next_state.level + 1 == state.level) {
            state = next_state;

            struct Object* object = get_object_by_id(output_object_id);
            object->type = FUNCTION_CALL_TYPE;
            object->OBJECT_FIELD(FUNCTION_CALL_TYPE) =  add_function_call();

            struct FunctionCall* functionCall = get_function_call_by_id(object->OBJECT_FIELD(FUNCTION_CALL_TYPE));
            functionCall->argc = i;
            for (arguments_size_t j = 0; j < i; ++j) {
                functionCall->arguments[j] = object_id[j];
            }
            return state;
        }
    }
    return get_invalid_state();
}

DEFINE_PARSE_TOKEN(ClosingBracket) {
    return state;
}

DEFINE_PARSE_TOKEN(Define) {
    return get_invalid_state();
}

DEFINE_PARSE_TOKEN(IncorrectToken) {
    return get_invalid_state();
}


struct State parse_object(PARSE_OBJECT_ARGS)
{
    if (state.type != READ_OBJECT_STATE)
        return get_invalid_state();
    struct Token token;
    state = next_state(state, reader, &token);
    if (state.type == INVALID_STATE)
        return state;

    switch (token.type) {
#define CASE(TYPE) case TYPE: return PARSE_TOKEN(TYPE)(state, reader, variables, output_object_id, &token);
        FOR_EACH_TOKEN_TYPE(CASE)
#undef CASE
    }
    return get_invalid_state();
}

struct State parse_definition(PARSE_DEFINITION_ARGS) {
    if (state.type != READ_DECLARATION_STATE)
        return get_invalid_state();
    struct Token function_name;
    state = next_state(state, reader, &function_name);
    CHECK_STATE(state);
    variables->argc = 0;

    struct Token token;
    for (arguments_size_t argc = 0; state.type != READ_OBJECT_STATE && argc + 1 < MAX_ARGUMENT_COUNT; ++argc) {
        struct State next = next_state(state, reader, &token);
        CHECK_STATE(next);
        state = next;
        if (state.type == READ_OBJECT_STATE)
            break;
        variables->argc = (arguments_size_t) (argc + 1);
        strcpy(variables->variables[argc], token.string);
    }

    add_function(function_name.string, variables->argc, object_id);

    return state;
}

struct Expression parse_expression(struct TokenStreamReader *reader) {
    struct State state = get_initial_state();
    struct Token token;
    struct Variables variables;
    state = next_state(state, reader, &token);
    struct Expression result;
    result.object_id = create_new_object();
    switch (state.type) {
        case READ_OBJECT_STATE:
            result.type = OBJECT_EXPRESSION;
            state = parse_object(state, reader, NULL, result.object_id);
            break;
        case READ_DECLARATION_STATE:
            result.type = DEFINITION_EXPRESSION;
            state = parse_definition(state, reader, &variables, result.object_id); // (define (...)
            state = parse_object(state, reader, &variables, result.object_id);
            state = next_state(state, reader, &token); // )
            break;
        default:
            return get_invalid_expression();
    }
    if (state.type == INVALID_STATE)
        return get_invalid_expression();
    return result;
}

struct Expression create_expression(enum ExpressionType type, object_id_t object_id) {
    struct Expression result = {
            .type = type,
            .object_id = object_id
    };
    return result;
}

struct Expression get_invalid_expression() {
    static struct Expression invalid = {
            .type = INVALID_EXPRESSION,
            .object_id = 0
    };
    return invalid;
}
