#include "test_framework.h"
#include "test_utils.h"

extern "C" {
#include "token.h"
}

TEST_SUITE(ReadToken) {
    TEST(OpeningBracket) {
        ASSERT_EQUAL(ReadToken("(+"), CreateToken(TokenType::OpeningBracket, "("));
    }


    TEST(ClosingBracket) {
        ASSERT_EQUAL(ReadToken(")..."), CreateToken(TokenType::ClosingBracket, ")"));
    }

    TEST(Number) {
        ASSERT_EQUAL(ReadToken("123."), CreateToken(TokenType::Number, "123"));
        ASSERT_EQUAL(ReadToken("-12_"), CreateToken(TokenType::Number, "-12"));
    }

    TEST(Operation) {
        ASSERT_EQUAL(ReadToken("+)"), CreateToken(TokenType::Operation, "+"));
        ASSERT_EQUAL(ReadToken("%*"), CreateToken(TokenType::Operation, "%"));
        ASSERT_EQUAL(ReadToken("-*"), CreateToken(TokenType::Operation, "-"));
        ASSERT_EQUAL(ReadToken("**"), CreateToken(TokenType::Operation, "*"));
        ASSERT_EQUAL(ReadToken("/*"), CreateToken(TokenType::Operation, "/"));
        ASSERT_EQUAL(ReadToken("<*"), CreateToken(TokenType::Operation, "<"));
        ASSERT_EQUAL(ReadToken(">*"), CreateToken(TokenType::Operation, ">"));
        ASSERT_EQUAL(ReadToken("=*"), CreateToken(TokenType::Operation, "="));
    }

    TEST(Name) {
        ASSERT_EQUAL(ReadToken("abc("), CreateToken(TokenType::Name, "abc"));
        ASSERT_EQUAL(ReadToken("a_("), CreateToken(TokenType::Name, "a_"));
        ASSERT_EQUAL(ReadToken("a_1("), CreateToken(TokenType::Name, "a_1"));
        ASSERT_EQUAL(ReadToken("abcdefghijkl]("),
                     CreateToken(TokenType::Name, "abcdefghijkl"));
    }

    TEST(Define) {
        ASSERT_EQUAL(ReadToken("define(+"), CreateToken(TokenType::Define, "define"));
    }

    TEST(IncorrectToken) {
        ASSERT_EQUAL(ReadToken("_abc"), CreateToken(TokenType::IncorrectToken, "_"));
        ASSERT_EQUAL(ReadToken("}abc"), CreateToken(TokenType::IncorrectToken, "}"));
        ASSERT_EQUAL(ReadToken("[abc"), CreateToken(TokenType::IncorrectToken, "["));
        ASSERT_EQUAL(ReadToken("@abc"), CreateToken(TokenType::IncorrectToken, "@"));
    }
}

TEST_SUITE(ReadAllTokens) {
    TEST(Example) {
        auto result = ReadAllTokens("(define(f)(+(function)-1))");
        std::vector<Token> expected = {
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Define, "define"),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Name, "f"),
                CreateToken(TokenType::ClosingBracket, ")"),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Operation, "+"),
                CreateToken(TokenType::OpeningBracket, "("),
                CreateToken(TokenType::Name, "function"),
                CreateToken(TokenType::ClosingBracket, ")"),
                CreateToken(TokenType::Number, "-1"),
                CreateToken(TokenType::ClosingBracket, ")"),
                CreateToken(TokenType::ClosingBracket, ")")
        };

        ASSERT_EQUAL(result, expected);
    }
}