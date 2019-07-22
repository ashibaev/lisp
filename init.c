#include "object.h"
#include "function_pool.h"
#include "simple_functions.h"

static void add_simple_functions() {
    add_simple_function("+", GET_LISP_FUNCTION_NAME(plus), 2);
    add_simple_function("-", GET_LISP_FUNCTION_NAME(minus), 2);
    add_simple_function("*", GET_LISP_FUNCTION_NAME(multiplies), 2);
    add_simple_function("/", GET_LISP_FUNCTION_NAME(divides), 2);
    add_simple_function("%", GET_LISP_FUNCTION_NAME(modulus), 2);
    add_simple_function("<", GET_LISP_FUNCTION_NAME(less), 2);
    add_simple_function(">", GET_LISP_FUNCTION_NAME(greater), 2);
    add_simple_function("=", GET_LISP_FUNCTION_NAME(equal_to), 2);
    add_simple_function("cons", GET_LISP_FUNCTION_NAME(cons), 2);
    add_simple_function("car", GET_LISP_FUNCTION_NAME(car), 1);
    add_simple_function("cdr", GET_LISP_FUNCTION_NAME(cdr), 1);
    add_simple_function("if", GET_LISP_FUNCTION_NAME(if), 3);
}


void init_all() {
    init_objects_pool();
    init_function_pool();
    add_simple_functions();
}

void clear_all() {
    clear_objects_pool();
    clear_function_pool();
}
