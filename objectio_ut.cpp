#include "test_framework.h"
#include "test_utils.h"

extern "C" {
#include "objectio.h"
#include "init.h"
#include "object.h"
#include "function_pool.h"
}

TEST_SUITE(Print) {
    TEST(PrintInteger) {
        init_all();

        char buffer[100];
        print_object_to_buffer(get_object_by_id(create_new_integer(10)), buffer);

        ASSERT_EQUAL(std::string(buffer), std::string("10"));

        clear_all();
    }

    TEST(Null) {
        init_all();

        char buffer[100];
        print_object_to_buffer(get_object_by_id(create_null_object()), buffer);

        ASSERT_EQUAL(std::string(buffer), std::string("null"));

        clear_all();
    }

    TEST(Pair) {
        init_all();

        char buffer[100];
        print_object_to_buffer(get_object_by_id(create_pair_object(
                create_new_integer(1),
                create_new_integer(2))), buffer);

        ASSERT_EQUAL(std::string(buffer), std::string("(cons 1 2)"));

        clear_all();
    }

    TEST(TwoCalls) {
        init_all();

        char buffer[100];
        char* new_buffer = print_object_to_buffer(get_object_by_id(create_pair_object(
                create_new_integer(1),
                create_new_integer(2))), buffer);
        new_buffer += sprintf(new_buffer, " ");
        print_object_to_buffer(get_object_by_id(create_null_object()), new_buffer);

        ASSERT_EQUAL(std::string(buffer), std::string("(cons 1 2) null"));

        clear_all();
    }
}

#define TEST_READ_OBJECT(INITIAL_STATE, EXPECTED_STATE, EXPECTED_OBJECT, VARIABLES, ...) do {       \
    auto state = INITIAL_STATE;                                                                         \
    auto id = create_new_object();                                                                      \
    auto reader = CreateTokenStreamReader({__VA_ARGS__});                                               \
    auto varibales = VARIABLES;                                                                         \
    auto finalState = parse_object(state, &reader, &varibales, id);                                     \
    ASSERT_EQUAL(finalState, EXPECTED_STATE);                                                           \
    if (finalState.type != INVALID_STATE)                                                               \
        ASSERT_EQUAL(*get_object_by_id(id), EXPECTED_OBJECT)  ;                                         \
} while (0)

TEST_SUITE(ReadInteger) {
    TEST(Positive) {
        init_all();
        Variables variables = CreateVariables({});
        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                CreateInteger(1),
                variables,
                CreateToken(TokenType::Number, "1")
        );
        clear_all();
    }

    TEST(Negative) {
        init_all();
        Variables variables = CreateVariables({});
        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                CreateInteger(-1),
                variables,
                CreateToken(TokenType::Number, "-1")
        );
        clear_all();
    }
}

TEST_SUITE(ReadOperation) {
    TEST(AllOperations) {
        init_all();

        std::vector<std::string> ops = {
                "+", "-", "*", "/", "%", "<", ">", "="
        };

        for (const auto& op : ops) {
            Variables variables = CreateVariables({});
            Object expected;
            expected.type = FUNCTION_TYPE;
            expected.OBJECT_FIELD(FUNCTION_TYPE) = get_function(op.c_str())->id;
            TEST_READ_OBJECT(
                    CreateState(1, READ_OBJECT_STATE),
                    CreateState(1, READ_OBJECT_STATE),
                    expected,
                    variables,
                    CreateToken(TokenType::Operation, op)
            );
        }

        clear_all();
    }
}

TEST_SUITE(ReadName) {
    TEST(ReadArgument) {
        init_all();

        Variables variables = CreateVariables({"a", "b", "c"});
        Object expected = *get_object_by_id(create_function_argument(1));
        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                expected,
                variables,
                CreateToken(TokenType::Name, "b")
        );

        clear_all();
    }

    TEST(ReadFunction) {
        init_all();

        Variables variables = CreateVariables({});
        Object expected = *get_object_by_id(create_function_object(add_function("func", 0, create_new_integer(2))));
        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                expected,
                variables,
                CreateToken(TokenType::Name, "func")
        );

        clear_all();
    }

    TEST(ReadVariableBeforeFunction) {
        init_all();

        Variables variables = CreateVariables({"a", "b", "func", "c"});
        create_function_object(add_function("func", 0, create_new_integer(2)));
        Object expected = *get_object_by_id(create_function_argument(2));
        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                expected,
                variables,
                CreateToken(TokenType::Name, "func")
        );

        clear_all();
    }

    TEST(CantResolveName) {
        init_all();

        Variables variables = CreateVariables({"a", "b", "func", "c"});
        create_function_object(add_function("func", 0, create_new_integer(2)));
        Object expected{};
        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                get_invalid_state(),
                expected,
                variables,
                CreateToken(TokenType::Name, "fun")
        );

        clear_all();
    }
}

TEST_SUITE(ReadBracket) {
    TEST(ReadSum) {
        // (+ 1 2)
        init_all();

        Variables variables = CreateVariables({});
        auto expected = *get_object_by_id(
                create_function_call_object(DefineFunctionCall("+", {create_new_integer(1), create_new_integer(2)}))
        );

        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                expected,
                variables,
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Operation, "+"),
                CreateToken(TokenType::Number, "1"),
                CreateToken(TokenType::Number, "2"),
                CreateToken(TokenType::ClosingBracket, ")")
        );

        clear_all();
    }

    TEST(ReadObjectWithVariables) {
        init_all();

        // (* (+ a b) c)
        Variables variables = CreateVariables({"a", "b", "c"});
        auto sum = create_function_call_object(
                DefineFunctionCall("+", {create_function_argument(0), create_function_argument(1)})
        );
        auto expected = create_function_call_object(
                DefineFunctionCall("*", {sum, create_function_argument(2)})
        );

        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                *get_object_by_id(expected),
                variables,
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Operation, "*"),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Operation, "+"),
                CreateToken(TokenType::Name, "a"),
                CreateToken(TokenType::Name, "b"),
                CreateToken(TokenType::ClosingBracket, ")"),
                CreateToken(TokenType::Name, "c"),
                CreateToken(TokenType::ClosingBracket, ")")
        );


        clear_all();
    }
}

TEST_SUITE(InvalidTokens) {
    TEST(1) {
        init_all();
        Variables variables = CreateVariables({});
        TEST_READ_OBJECT(
                CreateState(1, READ_OBJECT_STATE),
                get_invalid_state(),
                CreateInteger(-1),
                variables,
                CreateToken(TokenType::IncorrectToken, ".")
        );
        clear_all();
    }
}

TEST_SUITE(ReadVariables) {
    TEST(Simple) {
        init_all();
        Variables variables;
        auto initialState = CreateState(1, READ_DECLARATION_STATE);
        auto expectedState = CreateState(1, READ_OBJECT_STATE);
        auto expectedVariables = CreateVariables({"a", "b", "c"});
        auto reader = CreateTokenStreamReader({
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Name, "func"),
                CreateToken(TokenType::Name, "a"),
                CreateToken(TokenType::Name, "b"),
                CreateToken(TokenType::Name, "c"),
                CreateToken(TokenType::ClosingBracket, ")")
        });

        ASSERT_EQUAL(parse_definition(initialState, &reader, &variables, 1), expectedState);
        ASSERT_EQUAL(expectedVariables, variables);

        ASSERT_THAT(get_function("func") != NULL);

        clear_all();
    }
}


TEST_SUITE(ReadExpression) {
    TEST(Object) {
        init_all();

        // (+ (* 3 4) 5)
        auto reader = CreateTokenStreamReader({
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Operation, "+"),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Operation, "*"),
                CreateToken(TokenType::Number, "3"),
                CreateToken(TokenType::Number, "4"),
                CreateToken(TokenType::ClosingBracket, ")"),
                CreateToken(TokenType::Number, "5"),
                CreateToken(TokenType::ClosingBracket, ")")
        });
        auto result = parse_expression(&reader);

        auto mult = create_function_call_object(
                DefineFunctionCall("*", {create_new_integer(3), create_new_integer(4)})
        );

        auto sum = create_function_call_object(
                DefineFunctionCall("+", {mult, create_new_integer(5)})
        );

        ASSERT_EQUAL(result.type, OBJECT_EXPRESSION);
        ASSERT_EQUAL(*get_object_by_id(result.object_id), *get_object_by_id(sum));

        clear_all();
    }

    TEST(Function) {
        init_all();

        // (define(f a b)(* a (+ b a))
        auto reader = CreateTokenStreamReader({
                 CreateToken(TokenType::OpeningBracket, "("),
                 CreateToken(TokenType::Define, "define"),
                 CreateToken(TokenType::OpeningBracket, "("),
                 CreateToken(TokenType::Name, "f"),
                 CreateToken(TokenType::Name, "a"),
                 CreateToken(TokenType::Name, "b"),
                 CreateToken(TokenType::ClosingBracket, ")"),
                 CreateToken(TokenType::OpeningBracket, "("),
                 CreateToken(TokenType::Operation, "*"),
                 CreateToken(TokenType::Name, "a"),
                 CreateToken(TokenType::OpeningBracket, "("),
                 CreateToken(TokenType::Operation, "+"),
                 CreateToken(TokenType::Name, "b"),
                 CreateToken(TokenType::Name, "a"),
                 CreateToken(TokenType::ClosingBracket, ")"),
                 CreateToken(TokenType::ClosingBracket, ")"),
                 CreateToken(TokenType::ClosingBracket, ")")
        });
        auto result = parse_expression(&reader);

        ASSERT_EQUAL(result.type, DEFINITION_EXPRESSION);
        ASSERT_THAT(get_function("f") != NULL);

        clear_all();

    }

    TEST(Constant) {
        init_all();

        // (define N 5)
        auto reader = CreateTokenStreamReader({
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Define, "define"),
                CreateToken(TokenType::Name, "N"),
                CreateToken(TokenType::Number, "5"),
                CreateToken(TokenType::ClosingBracket, "("),
        });
        auto result = parse_expression(&reader);

        ASSERT_EQUAL(result.type, DEFINITION_EXPRESSION);
        ASSERT_THAT(get_function("N") != NULL);

        clear_all();
    }

    TEST(Invaid) {
        init_all();
        // (define (f a b))
        auto reader = CreateTokenStreamReader({
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Define, "define"),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Name, "f"),
                CreateToken(TokenType::Name, "a"),
                CreateToken(TokenType::Name, "b"),
                CreateToken(TokenType::ClosingBracket, ")"),
                CreateToken(TokenType::ClosingBracket, ")")
        });

        auto result = parse_expression(&reader);

        ASSERT_EQUAL(result.type, INVALID_EXPRESSION);

        clear_all();
    }
}