#include "state.h"

#define NEXT_STATE(NAME) next_state_ ## NAME
#define DEFINE_NEXT_STATE(NAME) struct State NEXT_STATE(NAME)(NEXT_STATE_ARGS)

struct State get_invalid_state() {
    static struct State result = {
            .level = 0,
            .type = INVALID_STATE
    };
    return result;
}

#define CHECK_IS_NULL(NAME, EXIT) do {if (!(NAME)) {EXIT} } while(0)
#define GET(NAME)  do {                                 \
    (NAME) = reader->get(reader);                       \
    CHECK_IS_NULL(NAME, return get_invalid_state(););   \
} while (0)

#define SEEK(NAME, NEXT) do {                           \
    (NAME) = reader->seek(reader, NEXT);                \
    CHECK_IS_NULL(NAME, return get_invalid_state(););   \
} while (0)

#define CHECK_TOKEN_TYPE(NAME, TYPE) CHECK_TOKEN_TYPE_IMPL(NAME, TYPE, return get_invalid_state();)
#define SET_RESULT(VALUE) do {if (result) *result = VALUE;} while (0)

DEFINE_NEXT_STATE(INITIAL_STATE) {
    struct Token const * token;
    SEEK(token, 0);
    CHECK_TOKEN_TYPE(token, OpeningBracket);

    SEEK(token, 1);
    if (token->type == Define) {
        GET(token);
        GET(token);
        return create_state(1, READ_DECLARATION_STATE);
    }

    return create_state(0, READ_OBJECT_STATE);
}

DEFINE_NEXT_STATE(READ_OBJECT_STATE) {
    struct Token const * token;
    GET(token);
    uint16_t next_level;
    SET_RESULT(*token);
    switch (token->type) {
        case OpeningBracket:
            return create_state((uint16_t) (state.level + 1), READ_OBJECT_STATE);
        case ClosingBracket:
            next_level = (uint16_t) (state.level - 1);
            return create_state(next_level, next_level ? READ_OBJECT_STATE : INITIAL_STATE);
        case IncorrectToken:
            return get_invalid_state();
        default:
            return state;
    }
}

DEFINE_NEXT_STATE(READ_DECLARATION_STATE) {
    struct Token const * token;
    GET(token);
    if (token->type == Name) {
        SET_RESULT(*token);
        return create_state(state.level, READ_OBJECT_STATE);
    }
    CHECK_TOKEN_TYPE(token, OpeningBracket);
    GET(token);
    CHECK_TOKEN_TYPE(token, Name);
    SET_RESULT(*token);
    return create_state((uint16_t) (state.level + 1), READ_VARIABLES_STATE);
}

DEFINE_NEXT_STATE(READ_VARIABLES_STATE) {
    struct Token const * token;
    GET(token);
    SET_RESULT(*token);
    switch (token->type) {
        case ClosingBracket:
            return create_state((uint16_t) (state.level - 1), READ_OBJECT_STATE);
        case Name:
            return state;
        default:
            return get_invalid_state();
    }
}

DEFINE_NEXT_STATE(INVALID_STATE) {
    return get_invalid_state();
}

struct State next_state(NEXT_STATE_ARGS) {
    switch (state.type) {
#define CASE(X) case X: return NEXT_STATE(X)(state, reader, result);
        FOR_EACH_STATE(CASE)
#undef CASE
    }
    return get_invalid_state();
}

struct State get_initial_state() {
    static struct State result ={
            .level = 0,
            .type = INITIAL_STATE
    };
    return result;
}

struct State create_state(uint16_t level, enum StateType type) {
    struct State result = {
            .level = level,
            .type = type
    };
    return result;
}
