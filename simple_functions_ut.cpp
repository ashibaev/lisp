#include <random>
#include <functional>

#include "test_framework.h"
#include "test_utils.h"


extern "C" {
#include "object.h"
#include "simple_functions.h"
}

static std::mt19937 rnd;


TEST_SUITE(SimpleFunctions) {

    void SimpleTest(int32_t first, int32_t second, Call function, int32_t expected) {
        auto firstInteger = CreateInteger(first);
        auto secondInteger = CreateInteger(second);
        Object const * args[2] = {&firstInteger, &secondInteger};
        Object result;

        function(&result, 2, args);

        ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), expected);
    }

    using Case = std::tuple<int32_t, int32_t, int32_t>;
    using Cases = std::vector<Case>;

    void SimpleTestCase(Call function, const std::function<int32_t(int32_t, int32_t)>& op, const Cases& cases) {
        for (const auto& test : cases) {
            SimpleTest(std::get<0>(test), std::get<1>(test), function, std::get<2>(test));
        }
        for (size_t i = 0; i < 100; ++i) {
            int32_t first = rnd() % 1000 + 1;
            int32_t second = rnd() % 1000 + 1;
            SimpleTest(first, second, function, op(first, second));
        }
    }

#define TEST_OPERATION(testName, op, cases)                     \
    TEST(testName) {                                            \
        auto function = GET_LISP_FUNCTION_NAME(op);             \
        SimpleTestCase(function, std::op<int32_t>(), cases);    \
    }

    TEST_OPERATION(Plus, plus, Cases({
        {1, 2, 3},
        {2, -2, 0},
        {1, -2, -1},
        {-2, 1, -1},
        {-3, 5, 2}
    }))

    TEST_OPERATION(Minus, minus, Cases({
        {1, 2, -1},
        {2, 1, 1},
        {1, 1, 0},
        {10, -20, 30},
        {-10, -20, 10},
        {-10, 12, -22}
    }))

    TEST_OPERATION(Multiply, multiplies, Cases({
        {2, 3, 6},
        {4, 5, 20},
        {2, -2, -4},
        {-2, -2, 4},
    }))

    TEST_OPERATION(Less, less, Cases({
        {1, 2, 1},
        {2, 1, 0},
        {1, 1, 0}
    }))

    TEST_OPERATION(Greater, greater, Cases({
        {1, 2, 0},
        {2, 1, 1},
        {1, 1, 0}
    }))

    TEST_OPERATION(Equal, equal_to, Cases({
        {1, 2, 0},
        {2, 1, 0},
        {1, 1, 1}
    }))

    TEST_OPERATION(Divides, divides, Cases({
        {10, 2, 5},
        {10, 3, 3},
        {10, 10, 1},
        {10, 11, 0},
        {-11, 2, -5},
        {11, -2, -5}
    }))

    TEST_OPERATION(Modulus, modulus, Cases({
        {10, 3, 1},
        {5, 10, 5},
        {11, -2, 1},
        {-11, 2, -1}
    }))

#undef TEST_OPERATION
}

TEST_SUITE(DivisionByZero) {
    TEST(Division) {
        auto function = GET_LISP_FUNCTION_NAME(divides);
        auto first = CreateInteger(3);
        auto second = CreateInteger(0);

        Object const * args[2] = {&first, &second};
        Object result;

        function(&result, 2, args);

        ASSERT_EQUAL(result.type, ObjectType::EXCEPTION_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(EXCEPTION_TYPE), ExceptionCode::DIVISION_BY_ZERO_ERROR);
    }

    TEST(Modulus) {
        auto function = GET_LISP_FUNCTION_NAME(divides);
        auto first = CreateInteger(3);
        auto second = CreateInteger(0);

        Object const * args[2] = {&first, &second};
        Object result;

        function(&result, 2, args);

        ASSERT_EQUAL(result.type, ObjectType::EXCEPTION_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(EXCEPTION_TYPE), ExceptionCode::DIVISION_BY_ZERO_ERROR);
    }
}


TEST_SUITE(Pairs) {
    Object GetObject() {
        return *get_object_by_id(create_new_object());
    }

    TEST(Construct) {
        init_objects_pool();

        auto function = GET_LISP_FUNCTION_NAME(cons);
        Object first = GetObject();
        Object second = GetObject();
        Object result;

        Object const * args[2] = {&first, &second};

        function(&result, 2, args);

        first.OBJECT_FIELD(INTEGER_TYPE) = 42;

        ASSERT_EQUAL(result.type, ObjectType::PAIR_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(PAIR_TYPE).first, first.id);
        ASSERT_EQUAL(result.OBJECT_FIELD(PAIR_TYPE).second, second.id);

        clear_objects_pool();
    }

    TEST(Getters) {
        init_objects_pool();

        auto function = GET_LISP_FUNCTION_NAME(cons);
        Object first = GetObject();
        Object second = GetObject();
        Object result;

        Object const * args[2] = {&first, &second};

        function(&result, 2, args);
        Object carResult, cdrResult;

        Object const * getterArgs[1] = {&result};

        GET_LISP_FUNCTION_NAME(car)(&carResult, 1, getterArgs);
        GET_LISP_FUNCTION_NAME(cdr)(&cdrResult, 1, getterArgs);

        ASSERT_EQUAL(carResult.id, first.id);
        ASSERT_EQUAL(cdrResult.id, second.id);

        clear_objects_pool();
     }
}
