#pragma once

#include <stddef.h>
#include <stdio.h>

#include "token.h"

#define CHECK_TOKEN_TYPE_IMPL(NAME, TYPE, EXIT)     \
do {                                                \
    if (token->type != TYPE) { EXIT }               \
} while (0)

#define TOKEN_STREAM_READER_BUFFER_SIZE 100
struct TokenStreamReader;

typedef struct Token const*(*GetOperation)(struct TokenStreamReader* tokenStreamReader);
typedef struct Token const*(*SeekOperation)(struct TokenStreamReader* tokenStreamReader, size_t next);

struct TokenStreamReader {
    struct Token buffer[TOKEN_STREAM_READER_BUFFER_SIZE];
    size_t begin;
    size_t end;
    FILE* file;
    GetOperation get;
    SeekOperation seek;
};

void init_stream_reader(struct TokenStreamReader* reader,
                        GetOperation getOperation,
                        SeekOperation seekOperation,
                        FILE* file);

struct Token const* get_next_token(struct TokenStreamReader* reader);

struct Token const* seek_next_token(struct TokenStreamReader* reader, size_t next);
