#include <string.h>
#include <stdlib.h>

#include "function_pool.h"
#include "vector_generic.h"
#include "vector.h"

#include <stdio.h>
#include <assert.h>

typedef uint16_t node_id_t;

struct Node {
    function_id_t function;
    node_id_t next[ALPHABET_SIZE];
};

DECLARE_VECTOR_INTERFACE(struct Node, Node)
DECLARE_VECTOR_IMPL(struct Node, Node)

static VECTOR_TYPE(Node)* Nodes = NULL;
static VECTOR_TYPE(Function)* Functions = NULL;
static VECTOR_TYPE(FunctionCall)* FunctionCalls = NULL;

static struct Node* Pool = NULL;

#define GET_NEXT_INDEX(NODE, STRING) (NODE)->next[get_position(*(STRING))]
#define GET_NODE(ID) VECTOR_FUNC(get_mutable_elem, Node)(Nodes, ID)
#define GET_NEXT_NODE(NODE, STRING) GET_NODE(GET_NEXT_INDEX(NODE, STRING))

#define GET_FUNCTION(ID) VECTOR_FUNC(get_mutable_elem, Function)(Functions, ID)

static uint8_t get_position(char x) {
    return (uint8_t) (x - 32);
}


static node_id_t add_node() {
    Nodes = VECTOR_FUNC(add_element, Node)(Nodes);
    struct Node* back = VECTOR_FUNC(mutable_back, Node)(Nodes);
    memset(back, 0, sizeof(struct Node));
    back->function = INVALID_FUNCTION_ID;
    Pool = VECTOR_FUNC(get_mutable_elem, Node)(Nodes, 0);
    return (node_id_t) (Nodes->size - 1);
}

static struct Node* find_node(const char* name) {
    if (name == NULL) {
        return NULL;
    }
    size_t len = 0;
    node_id_t current_node_id = 0;
    for (; *name; ++name, ++len) {
        if (!GET_NEXT_INDEX(GET_NODE(current_node_id), name))
            return NULL;
        current_node_id = GET_NEXT_INDEX(GET_NODE(current_node_id), name);
    }
    return len ? GET_NODE(current_node_id) : NULL;
}

static struct Node* create_node(const char* name) {
    if (name == NULL) {
        return NULL;
    }
    size_t len = 0;
    node_id_t current_node_id = 0;
    for (; *name; ++name, ++len) {
        if (!GET_NEXT_INDEX(GET_NODE(current_node_id), name)) {
            node_id_t next_node = add_node();
            GET_NEXT_INDEX(GET_NODE(current_node_id), name) = next_node;
        }
        current_node_id = GET_NEXT_INDEX(GET_NODE(current_node_id), name);
    }
    return len ? GET_NODE(current_node_id): NULL;
}

struct Function* get_function(const char* name) {
    struct Node * current = find_node(name);
    return (current && current->function != INVALID_FUNCTION_ID)
           ? GET_FUNCTION(current->function)
           : NULL;
}

struct Function* get_function_by_id(function_id_t id) {
    return GET_FUNCTION(id);
}

uint16_t add_new_simple_function(Call functionCall, arguments_size_t argc) {
    Functions = VECTOR_FUNC(add_element, Function)(Functions);
    struct Function* current_function = VECTOR_FUNC(mutable_back, Function)(Functions);
    current_function->type = SIMPLE_FUNCTION;
    current_function->arguments_amount = argc;
    current_function->call = functionCall;
    return current_function->id = (uint16_t) (Functions->size - 1);
}

bool add_simple_function(const char *name, Call function, arguments_size_t argc) {
    struct Node* current_node = create_node(name);
    if (current_node == NULL || current_node->function != INVALID_FUNCTION_ID)
        return false;
    current_node->function = add_new_simple_function(function, argc);
    return true;
}

function_id_t add_function(const char* name, arguments_size_t argc, object_id_t object_id) {
    struct Node* current_node = create_node(name);
    if (current_node == NULL || current_node->function != INVALID_FUNCTION_ID)
        return INVALID_FUNCTION_ID;
    Functions = VECTOR_FUNC(add_element, Function)(Functions);
    struct Function* current_function = VECTOR_FUNC(mutable_back, Function)(Functions);
    current_function->type = COMPOSITE_FUNCTION;
    current_function->arguments_amount = argc;
    current_function->object_id = object_id;
    return current_function->id = current_node->function = (function_id_t) (Functions->size - 1);
}

function_call_id_t add_function_call() {
    FunctionCalls = VECTOR_FUNC(add_element, FunctionCall)(FunctionCalls);
    return (function_call_id_t) (FunctionCalls->size - 1);
}


void init_function_pool() {
    Nodes = VECTOR_FUNC(empty, Node)();
    Functions = VECTOR_FUNC(empty, Function)();
    FunctionCalls = VECTOR_FUNC(empty, FunctionCall)();
    add_node();

    Pool = VECTOR_FUNC(mutable_back, Node)(Nodes);
}

void clear_function_pool() {
    VECTOR_FUNC(free, Node)(Nodes);
    VECTOR_FUNC(free, Function)(Functions);
    VECTOR_FUNC(free, FunctionCall)(FunctionCalls);
}

struct FunctionCall *get_function_call_by_id(function_call_id_t id) {
    return VECTOR_FUNC(get_mutable_elem, FunctionCall)(FunctionCalls, id);
}

