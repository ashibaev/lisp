#include "test_framework.h"

extern "C" {
#include "function_pool.h"
#include "simple_functions.h"
}

TEST_SUITE(AddSimple) {
    TEST(Simple) {
        init_function_pool();

        ASSERT_THAT(add_simple_function("+", GET_LISP_FUNCTION_NAME(plus), 2));

        ASSERT_EQUAL(get_function("+")->call, GET_LISP_FUNCTION_NAME(plus));
        ASSERT_EQUAL(get_function("+")->arguments_amount, 2);
        ASSERT_EQUAL(get_function_by_id(0), get_function("+"));

        clear_function_pool();
    }

    TEST(DontAddTwice) {
        init_function_pool();

        ASSERT_THAT(add_simple_function("+", GET_LISP_FUNCTION_NAME(plus), 2));
        ASSERT_THAT_NOT(add_simple_function("+", GET_LISP_FUNCTION_NAME(minus), 2));
        ASSERT_EQUAL(get_function("+")->call, GET_LISP_FUNCTION_NAME(plus));
        ASSERT_EQUAL(get_function("+")->arguments_amount, 2);

        clear_function_pool();
    }

    TEST(DontAddEmpty) {
        init_function_pool();

        ASSERT_THAT_NOT(add_simple_function("", GET_LISP_FUNCTION_NAME(plus), 2));
        ASSERT_THAT_NOT(add_simple_function(NULL, GET_LISP_FUNCTION_NAME(plus), 2));

        clear_function_pool();
    }

    TEST(AddLongName) {
        init_function_pool();

        ASSERT_THAT(add_simple_function("plus", GET_LISP_FUNCTION_NAME(plus), 2));
        ASSERT_THAT(add_simple_function("pluss", GET_LISP_FUNCTION_NAME(minus), 2));
        ASSERT_THAT(add_simple_function("plu", GET_LISP_FUNCTION_NAME(multiplies), 2));

        ASSERT_EQUAL(get_function("plus"), get_function_by_id(0));
        ASSERT_EQUAL(get_function("pluss"), get_function_by_id(1));
        ASSERT_EQUAL(get_function("plu"), get_function_by_id(2));

        clear_function_pool();
    }
}