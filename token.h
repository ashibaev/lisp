#pragma once

#include <stdio.h>
#include <stdint.h>

#define FOR_EACH_TOKEN_TYPE(expr)   \
expr(OpeningBracket) expr(ClosingBracket) expr(Number) expr(Define) expr(Name) expr(Operation) expr(IncorrectToken)

#define MAX_TOKEN_LENGTH 31

enum TokenType {
#define DEF(X) X,
    FOR_EACH_TOKEN_TYPE(DEF)
#undef DEF
};

struct Token {
    char string[MAX_TOKEN_LENGTH];
    enum TokenType type : 8;
};

const char* read_token(struct Token* result, const char* input);

typedef struct Token* (*NextIt)(struct Token* it);

size_t read_all_tokens(struct Token* output_it, NextIt next, const char* input);
