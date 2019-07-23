#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "token.h"

#define TRY_PARSE(NAME) {                               \
    input = READER(NAME)(result, input, &success);      \
    if (success) return input;                          \
}

#define READER(NAME) read_ ##NAME
#define DEFINE_TOKEN_READER(NAME) const char* READER(NAME) (struct Token* result, char const* input, bool* success)\

#define DEFINE_ONE_SYMBOL_READER(NAME, CNT, SYMBOLS)                \
DEFINE_TOKEN_READER(NAME) {                                         \
    *success = false;                                               \
    if (!input)                                                     \
        return input;                                               \
    char symbols[(CNT) + 1] = SYMBOLS;                              \
    for (size_t i = 0; i < (CNT); ++i) {                            \
        if (*input == symbols[i]) {                                 \
            result->type = NAME;                                    \
            result->string[0] = symbols[i];                         \
            result->string[1] = 0;                                  \
            *success = true;                                        \
            return input + 1;                                       \
        }                                                           \
    }                                                               \
    return input;                                                   \
}

DEFINE_ONE_SYMBOL_READER(OpeningBracket, 1, "(")
DEFINE_ONE_SYMBOL_READER(ClosingBracket, 1, ")")
DEFINE_ONE_SYMBOL_READER(Operation, 8, "+-*/%<>=")

#define CHECK_SYMBOL(NAME) check_ ## NAME ## _symbol
#define DEFINE_CHECK_SYMBOL(NAME, FIRST, OTHER) bool CHECK_SYMBOL(NAME)(char const* input, size_t position) {   \
    if (!input)                                                                                                 \
        return false;                                                                                           \
    if (position)                                                                                               \
        return OTHER;                                                                                           \
    return FIRST;                                                                                               \
}

DEFINE_CHECK_SYMBOL(
        Number,
        isdigit(*input) || (*input == '-' && isdigit(*(input + 1))),
        isdigit(*input)
)

DEFINE_CHECK_SYMBOL(
        Name,
        isalpha(*input),
        isalnum(*input) || *input == '_'
)

DEFINE_CHECK_SYMBOL(
        Define,
        *input == 'd',
        *input == "define"[position];
)

#define DEFINE_READER_WITH_POSITIONAL_CHECKER(NAME)                                 \
DEFINE_TOKEN_READER(NAME) {                                                         \
    *success = false;                                                               \
    if (!input)                                                                     \
        return input;                                                               \
    size_t pos;                                                                     \
    for (pos = 0; *input && CHECK_SYMBOL(NAME)(input, pos); ++pos, ++input) {       \
        *success = true;                                                            \
        result->type = NAME;                                                        \
        result->string[pos] = *input;                                               \
    }                                                                               \
    result->string[pos] = 0;                                                        \
    return input;                                                                   \
}

DEFINE_READER_WITH_POSITIONAL_CHECKER(Name)
DEFINE_READER_WITH_POSITIONAL_CHECKER(Number)
DEFINE_READER_WITH_POSITIONAL_CHECKER(Define)

DEFINE_TOKEN_READER(IncorrectToken){
    *success = true;
    result->type = IncorrectToken;
    result->string[0] = *input;
    result->string[1] = 0;
    return input + 1;
}

const char* read_token(struct Token* result, char const* input) {
    bool success = false;
    FOR_EACH_TOKEN_TYPE(TRY_PARSE)
    return input;
}

size_t read_all_tokens(struct Token *output_it, NextIt next, const char *input) {
    size_t read = 0;
    for (;input && *input; ++read, output_it = next(output_it)) {
        input = read_token(output_it, input);
    }
    return read;
}
