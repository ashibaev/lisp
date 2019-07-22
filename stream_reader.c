#include <string.h>
#include <stdbool.h>
#include "stream_reader.h"

size_t next_position(size_t position, size_t cnt) {
    size_t next = position + cnt;
    return next % TOKEN_STREAM_READER_BUFFER_SIZE;
}

size_t distance(size_t first, size_t second) {
    return first <= second
           ? (second - first)
           : (TOKEN_STREAM_READER_BUFFER_SIZE - first + second);
}

struct Token * add_token(struct TokenStreamReader* reader) {
    struct Token* result =  &reader->buffer[reader->end];
    reader->end = next_position(reader->end, 1);
    return result;
}

static char buffer[1000];

bool read_tokens(struct TokenStreamReader* reader) {
    if (!fscanf(reader->file, "%980s", buffer))
        return false;


    size_t len = strlen(buffer);

    if (len > 900) {
        return false;
    }
    for (char const * current_pos = buffer; current_pos && *current_pos;) {
        current_pos = read_token(add_token(reader), current_pos);
    }
    fscanf(reader->file, "%*[^-+*=<>%//0-9a-zA-Z()]");
    return true;
}

struct Token const* get_next_token(struct TokenStreamReader* reader) {
    if (reader->begin == reader->end && !read_tokens(reader)) {
        return NULL;
    }
    return &reader->buffer[reader->begin++];
}

struct Token const* seek_next_token(struct TokenStreamReader* reader, size_t next) {
    if (next > 5) {
        return NULL;
    }
    while (distance(reader->begin, reader->end) <= next) {
        if (!read_tokens(reader))
            return NULL;
    }
    return &reader->buffer[next_position(reader->begin, next)];
}

void init_stream_reader(struct TokenStreamReader *reader,
                        GetOperation getOperation,
                        SeekOperation seekOperation,
                        FILE *file)
{
    reader->get = getOperation;
    reader->seek = seekOperation;
    reader->file = file;
    reader->begin = reader->end = 0;
}

