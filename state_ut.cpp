#include "test_framework.h"
#include "test_utils.h"

extern "C" {
#include "state.h"
#include "token.h"
}

#define CHECK_NEXT_STATE_WITH_RESULT(INIT, EXPECTED, RESULT, ...) do {                          \
    auto state = INIT;                                                                          \
    auto reader = CreateTokenStreamReader({__VA_ARGS__});                                       \
    auto expected = EXPECTED;                                                                   \
    ASSERT_EQUAL(next_state(state, &reader, RESULT), expected);                                 \
} while (0)

#define CHECK_NEXT_STATE(INIT, EXPECTED, ... ) CHECK_NEXT_STATE_WITH_RESULT(INIT, EXPECTED, NULL, __VA_ARGS__)


TEST_SUITE(INITIAL) {
    TEST(Simple) {
        auto state = get_initial_state();
        ASSERT_EQUAL(state.level, 0);
        ASSERT_EQUAL(state.type, StateType::INITIAL_STATE);
    }

    TEST(ToReadExpression) {
        CHECK_NEXT_STATE(
                get_initial_state(),
                CreateState(0, READ_OBJECT_STATE),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Name, "abc")
        );
    }

    TEST(ToReadExpressionWithIncorrectNextToken) {
        CHECK_NEXT_STATE(
                get_initial_state(),
                CreateState(0, READ_OBJECT_STATE),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::IncorrectToken, "!")
        );
    }

    TEST(ToReadDeclaration) {
        CHECK_NEXT_STATE(
                get_initial_state(),
                CreateState(1, READ_DECLARATION_STATE),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Define, "define"),
                CreateToken(TokenType::OpeningBracket, "(")
        );

        CHECK_NEXT_STATE(
                get_initial_state(),
                CreateState(1, READ_DECLARATION_STATE),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Define, "define"),
                CreateToken(TokenType::OpeningBracket, "N"),
                CreateToken(TokenType::Number, "5"),
                CreateToken(TokenType::ClosingBracket, ")")
        );
    }

    TEST(ReadSomthingIncorrect) {
        CHECK_NEXT_STATE(
                get_initial_state(),
                GetInvalidState(),
                CreateToken(TokenType::Number, "12")
        );

        CHECK_NEXT_STATE(
                get_initial_state(),
                GetInvalidState(),
                CreateToken(TokenType::Operation, "+")
        );


        CHECK_NEXT_STATE(
                get_initial_state(),
                GetInvalidState(),
                CreateToken(TokenType::Name, "f")
        );


        CHECK_NEXT_STATE(
                get_initial_state(),
                GetInvalidState(),
                CreateToken(TokenType::Define, "define")
        );

        CHECK_NEXT_STATE(
                get_initial_state(),
                GetInvalidState(),
                CreateToken(TokenType::ClosingBracket, ")")
        );
    }
}

TEST_SUITE(READ_EXPRESSION) {
    TEST(ReadOpeningBracket) {
        CHECK_NEXT_STATE(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(2, READ_OBJECT_STATE),
                CreateToken(TokenType::OpeningBracket, "(")
        );
    }

    TEST(ReadClosingBracket) {
        CHECK_NEXT_STATE(
                CreateState(2, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                CreateToken(TokenType::ClosingBracket, ")")
        );

        CHECK_NEXT_STATE(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(0, INITIAL_STATE),
                CreateToken(TokenType::ClosingBracket, ")")
        );
    }

    TEST(ReadName) {
        Token result;
        CHECK_NEXT_STATE_WITH_RESULT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                &result,
                CreateToken(TokenType::Name, "abc")
        );
        ASSERT_EQUAL(result, CreateToken(TokenType::Name, "abc"));
    }

    TEST(ReadOperation) {
        Token result;
        CHECK_NEXT_STATE_WITH_RESULT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                &result,
                CreateToken(TokenType::Operation, "+")
        );
        ASSERT_EQUAL(result, CreateToken(TokenType::Operation, "+"));
    }

    TEST(ReadNumber) {
        Token result;
        CHECK_NEXT_STATE_WITH_RESULT(
                CreateState(1, READ_OBJECT_STATE),
                CreateState(1, READ_OBJECT_STATE),
                &result,
                CreateToken(TokenType::Number, "12")
        );
        ASSERT_EQUAL(result, CreateToken(TokenType::Number, "12"));
    }

    TEST(ReadDefine) {
        CHECK_NEXT_STATE(
                CreateState(1, READ_OBJECT_STATE),
                GetInvalidState(),
                CreateToken(TokenType::IncorrectToken, "!")
        );
    }

    TEST(ReadIncorrect) {
        CHECK_NEXT_STATE(
                CreateState(1, READ_OBJECT_STATE),
                GetInvalidState(),
                CreateToken(TokenType::IncorrectToken, "!")
        );
    }
}

TEST_SUITE(READ_DECLARATION) {
    TEST(ReadName) {
        Token result;
        CHECK_NEXT_STATE_WITH_RESULT(
                CreateState(1, READ_DECLARATION_STATE),
                CreateState(2, READ_VARIABLES_STATE),
                &result,
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Name, "func")
        );
        ASSERT_EQUAL(result, CreateToken(TokenType::Name, "func"));
    }

    TEST(ReadOther) {
        std::vector<Token> tokens = {
                CreateToken(TokenType::IncorrectToken, "!"),
                CreateToken(TokenType::ClosingBracket, ")"),
                CreateToken(TokenType::Number, "-12"),
                CreateToken(TokenType::Operation, "+"),
                CreateToken(TokenType::Define, "define")
        };

        for (const auto& token : tokens) {
            CHECK_NEXT_STATE(
                    CreateState(1, READ_DECLARATION_STATE),
                    GetInvalidState(),
                    token
            );
        }
    }
}

TEST_SUITE(READ_VARIABLES) {
    TEST(ReadBracket) {
        CHECK_NEXT_STATE(
                CreateState(2, READ_VARIABLES_STATE),
                GetInvalidState(),
                CreateToken(TokenType::OpeningBracket, "(")
        );

        CHECK_NEXT_STATE(
                CreateState(2, READ_VARIABLES_STATE),
                CreateState(1, READ_OBJECT_STATE),
                CreateToken(TokenType::ClosingBracket, ")")
        );
    }

    TEST(ReadNumber) {
        CHECK_NEXT_STATE(
                CreateState(2, READ_VARIABLES_STATE),
                GetInvalidState(),
                CreateToken(TokenType::Number, "12")
        );
    }

    TEST(ReadOperation) {
        CHECK_NEXT_STATE(
                CreateState(2, READ_VARIABLES_STATE),
                GetInvalidState(),
                CreateToken(TokenType::Operation, "+")
        );
    }

    TEST(ReadName) {
        Token result;
        CHECK_NEXT_STATE_WITH_RESULT(
                CreateState(2, READ_VARIABLES_STATE),
                CreateState(2, READ_VARIABLES_STATE),
                &result,
                CreateToken(TokenType::Name, "abc")
        );
        ASSERT_EQUAL(result, CreateToken(TokenType::Name, "abc"));
    }

    TEST(ReadDefine) {
        CHECK_NEXT_STATE(
                CreateState(2, READ_VARIABLES_STATE),
                GetInvalidState(),
                CreateToken(TokenType::Define, "define")
        );
    }
}