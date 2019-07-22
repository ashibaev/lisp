#pragma once

#include "object.h"

#define GET_LISP_FUNCTION_NAME(NAME) NAME##_lisp_function
#define DEFINE_FUNCTION(NAME)       \
void GET_LISP_FUNCTION_NAME(NAME)(struct Object* result, arguments_size_t argc, struct Object const ** args)

DEFINE_FUNCTION(cons);
DEFINE_FUNCTION(car);
DEFINE_FUNCTION(cdr);
DEFINE_FUNCTION(if);

DEFINE_FUNCTION(plus);
DEFINE_FUNCTION(minus);
DEFINE_FUNCTION(divides);
DEFINE_FUNCTION(multiplies);
DEFINE_FUNCTION(modulus);
DEFINE_FUNCTION(less);
DEFINE_FUNCTION(greater);
DEFINE_FUNCTION(equal_to);
