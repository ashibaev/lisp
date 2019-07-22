#pragma once

#include <stdint.h>
#include "stream_reader.h"

#define FOR_EACH_STATE(expr) \
expr(INITIAL_STATE) expr(READ_OBJECT_STATE) expr(READ_DECLARATION_STATE) expr(READ_VARIABLES_STATE) expr(INVALID_STATE)

enum StateType {
#define DEF(X) X,
    FOR_EACH_STATE(DEF)
#undef DEF
};


struct State {
    uint16_t level;
    enum StateType type : 16;
};


#define NEXT_STATE_ARGS             \
struct State state,                 \
struct TokenStreamReader* reader,   \
struct Token* result

struct State get_initial_state();
struct State get_invalid_state();
struct State create_state(uint16_t level, enum StateType type);
struct State next_state(NEXT_STATE_ARGS);
