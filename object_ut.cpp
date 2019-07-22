#include "test_framework.h"
#include "test_utils.h"

extern "C" {
#include "init.h"
#include "object.h"
#include "function_pool.h"
}

TEST_SUITE(ObjectPool) {
    TEST(IncreaseId) {
        init_all();
        uint16_t first = create_new_object();
        uint16_t second = create_new_object();

        ASSERT_EQUAL(first + 1, second);

        clear_all();
    }

    TEST(Initialized) {
        init_all();
        uint16_t first = create_new_object();
        uint16_t second = create_new_object();

        ASSERT_EQUAL(get_object_by_id(first)->id, first);
        ASSERT_EQUAL(get_object_by_id(second)->id, second);

        clear_all();
    }

    TEST(CreateInteger) {
        init_all();
        uint16_t first = create_new_integer(42);

        ASSERT_EQUAL(get_object_by_id(first)->type, ObjectType::INTEGER_TYPE);
        ASSERT_EQUAL(get_object_by_id(first)->OBJECT_FIELD(INTEGER_TYPE), 42);

        clear_all();
    }

    TEST(NullIsTheSame) {
        init_all();

        ASSERT_EQUAL(create_null_object(), create_null_object());

        clear_all();
    }
}

TEST_SUITE(Evaluating) {
    TEST(Integer) {
        init_all();

        Object* first = get_object_by_id(create_new_integer(111));
        Object result;

        get_value(&result, first, 0, NULL);
        ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 111);

        clear_all();
    }

    TEST(Null) {
        init_all();

        Object* null = get_object_by_id(create_null_object());
        Object result;

        get_value(&result, null, 0, NULL);

        ASSERT_EQUAL(result.type, ObjectType::NULL_TYPE);

        clear_all();
    }

    TEST(Pair) {
        init_all();
        uint16_t first = create_new_object();
        uint16_t second = create_new_object();

        struct Object* pair = get_object_by_id(create_new_object());
        pair->type = ObjectType::PAIR_TYPE;
        pair->OBJECT_FIELD(PAIR_TYPE) = {first, second};

        Object result;

        get_value(&result, pair, 0, NULL);

        ASSERT_EQUAL(result.type, ObjectType::PAIR_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(PAIR_TYPE).first, first);
        ASSERT_EQUAL(result.OBJECT_FIELD(PAIR_TYPE).second, second);

        clear_all();
    }

    TEST(Function) {
        init_all();

        struct Object* function = get_object_by_id(create_new_object());
        function->type = ObjectType::FUNCTION_TYPE;
        function->OBJECT_FIELD(FUNCTION_TYPE) = 1;

        Object result;

        get_value(&result, function, 0, NULL);

        ASSERT_EQUAL(result.type, ObjectType::FUNCTION_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(FUNCTION_TYPE), function->OBJECT_FIELD(FUNCTION_TYPE));

        clear_all();
    }

    TEST(Exception) {
        init_all();

        struct Object* exception = get_object_by_id(create_new_object());
        exception->type = ObjectType::EXCEPTION_TYPE;
        exception->OBJECT_FIELD(EXCEPTION_TYPE) = ExceptionCode::RUNTIME_ERROR;

        Object result;

        get_value(&result, exception, 0, NULL);

        ASSERT_EQUAL(result.type, ObjectType::EXCEPTION_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(EXCEPTION_TYPE), exception->OBJECT_FIELD(EXCEPTION_TYPE));

        clear_all();
    }

    TEST(FunctionArgument) {
        init_all();

        struct Object* argument = get_object_by_id(create_function_argument(1));

        auto firstId = create_new_integer(10);
        auto secondId = create_new_integer(20);

        Object const * args[10] = {
                get_object_by_id(firstId),
                get_object_by_id(secondId)
        };

        Object result;

        get_value(&result, argument, 2, args);
        ASSERT_EQUAL(result, *args[1]);

        clear_all();
    }
}


TEST_SUITE(EvaluateCall) {

    void SimpleTest(int32_t first,
                    int32_t second,
                    const std::string& name,
                    const std::function<int32_t(int32_t, int32_t)>& op)
    {
        {
            auto firstId = create_new_integer(first);
            auto secondId = create_new_integer(second);
            auto objectId = create_function_call_object(DefineFunctionCall(name, {firstId, secondId}));

            Object result;

            get_value(&result, get_object_by_id(objectId), 0, NULL);

            ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
            ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), op(first, second));
        }
    }


    TEST(Arithmetic) {
        init_all();

        SimpleTest(1, 2, "+", std::plus<int32_t>());      // (+ 1 2)
        SimpleTest(5, 3, "-", std::minus<int32_t>());     // (- 5 3)
        SimpleTest(10, 2, "/", std::divides<int32_t>());  // (/ 10 2)
        SimpleTest(3, 2, ">", std::greater<int32_t>());   // (> 3 2)

        // (+ 2 (* 3 4))
        {
            auto firstId = create_new_integer(2);
            auto secondId = create_new_integer(3);
            auto thirdId = create_new_integer(4);

            auto multiplicationObjectId = create_function_call_object(DefineFunctionCall("*", {secondId, thirdId}));
            auto sumObjectId = create_function_call_object(DefineFunctionCall("+", {firstId, multiplicationObjectId}));

            Object result;

            get_value(&result, get_object_by_id(sumObjectId), 0, NULL);

            ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
            ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 14);
        }


        clear_all();
    }

    TEST(Condition) {
        init_all();
        // (if (= 1 1) 2 3)

        auto firstOneId = create_new_integer(1);
        auto secondOneId = create_new_integer(1);
        auto twoId = create_new_integer(2);
        auto threeId = create_new_integer(3);

        auto checkId = create_function_call_object(DefineFunctionCall("=", {firstOneId, secondOneId}));
        auto condId = create_function_call_object(DefineFunctionCall("if", {checkId, twoId, threeId}));

        Object result;

        get_value(&result, get_object_by_id(condId), 0, NULL);

        ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 2);

        clear_all();
    }

    TEST(ArgmuentSubstitution) {
        init_all();
        // (define (f x) (* 2 (+ x 1))) -> (* 2 (+ FUNCTION_ARGUMENT(0) 1))

        auto oneId = create_new_integer(1);
        auto twoId = create_new_integer(2);

        auto functionArgId = create_function_argument(0);
        auto sumId = create_function_call_object(DefineFunctionCall("+", {functionArgId, oneId}));
        auto multId = create_function_call_object(DefineFunctionCall("*", {twoId, sumId}));

        add_function("f", 1, multId);

        {
            Object result;
            Object const * args[10] = {
                    get_object_by_id(create_new_integer(3))
            };

            get_value(&result, get_object_by_id(multId), 1, args);

            ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
            ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 8);
        }

        {
            Object result;
            Object const * args[10] = {
                    get_object_by_id(create_new_integer(0))
            };

            get_value(&result, get_object_by_id(multId), 1, args);

            ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
            ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 2);
        }

        {
            Object result;
            Object const * args[10] = {
                    get_object_by_id(create_new_integer(1))
            };

            get_value(&result, get_object_by_id(multId), 1, args);

            ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
            ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 4);
        }

        {
            // (f 3)
            auto callObject = create_function_call_object(DefineFunctionCall("f", { create_new_integer(3)} ));
            Object result;

            get_value(&result, get_object_by_id(callObject), 0, NULL);

            ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
            ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 8);
        }

        clear_all();
    }


    TEST(CompositeFunction) {
        init_all();
        // (define (f x y z) (x y z))

        auto firstId = create_function_argument(0);
        auto secondId = create_function_argument(1);
        auto thirdId = create_function_argument(2);

        auto func = create_function_call_object(DefineFunctionCall({firstId, secondId, thirdId}));
        auto plus = create_function_object(get_object_by_id('+')->id);

        // (f + 10 2)

        Object const * args[10] = {
                get_object_by_id(plus),
                get_object_by_id(create_new_integer(10)),
                get_object_by_id(create_new_integer(2))
        };

        Object result;

        get_value(&result, get_object_by_id(func), 3, args);

        ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 12);

        clear_all();
    }

    TEST(NumberInBrackets) {
        init_all();
        // (1)
        auto object_id = create_function_call_object(DefineFunctionCall({create_new_integer(1)}));

        Object result;
        get_value(&result, get_object_by_id(object_id), 0, NULL);

        ASSERT_EQUAL(result, CreateInteger(1));

        clear_all();
    }

    TEST(Constant) {
        init_all();

        auto one = create_new_integer(1);
        add_function("f", 0, one);
        auto call = create_function_call_object(DefineFunctionCall("f", {}));

        Object result;
        get_value(&result, get_object_by_id(call), 0, NULL);

        ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
        ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), 1);

        clear_all();
    }

    TEST(Factorial){
        init_all();

        //(define (f n) (if (= n 0) 1 (* (f (- n 1)) n)))

        auto top_id = create_new_object();
        add_function("f", 1, top_id);

        auto prev_n = create_function_call_object(DefineFunctionCall("-", {
            create_function_argument(0),
            create_new_integer(1)
        }));

        auto prev_fact = create_function_call_object(DefineFunctionCall("f", {prev_n}));

        auto mult = create_function_call_object(DefineFunctionCall("*", {
            prev_fact,
            create_function_argument(0)
        }));

        auto check = create_function_call_object(DefineFunctionCall("=", {
            create_function_argument(0),
            create_new_integer(0)
        }));

        auto ifCall = DefineFunctionCall("if", {check, create_new_integer(1), mult});
        struct Object* object = get_object_by_id(top_id);
        object->type = FUNCTION_CALL_TYPE;
        object->OBJECT_FIELD(FUNCTION_CALL_TYPE) = ifCall;

        std::vector<int32_t> answers = {1, 1, 2, 6, 24, 120, 720};
        for (size_t i = 0; i < answers.size(); ++i) {
            Object result;
            Object const * args[10] = {
                    get_object_by_id(create_new_integer(i))
            };
            get_value(&result, object, 1, args);

            ASSERT_EQUAL(result.type, ObjectType::INTEGER_TYPE);
            ASSERT_EQUAL(result.OBJECT_FIELD(INTEGER_TYPE), answers[i]);
        }

        clear_all();
    }
}