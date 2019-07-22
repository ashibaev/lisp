#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "object.h"
#include "id_types.h"

#define ALPHABET_SIZE 100

#define INVALID_FUNCTION_ID ((function_id_t)-1)
#define INVALID_FUNCTION_CALL_ID ((function_call_id_t) -1)

struct Function* get_function(const char* name);
struct Function* get_function_by_id(function_id_t id);

struct FunctionCall* get_function_call_by_id(function_call_id_t id);

bool add_simple_function(const char *name, Call function, arguments_size_t argc);

function_id_t add_function(const char* name, arguments_size_t argc, object_id_t object_id);
function_call_id_t add_function_call();

void init_function_pool();
void clear_function_pool();
