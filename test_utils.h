#include <iostream>
#include <vector>

extern "C" {
#include "state.h"
#include "token.h"
#include "object.h"
#include "objectio.h"
}


State CreateState(uint16_t level, StateType type);
State GetInvalidState();

bool operator == (const State& lhs, const State& rhs);
bool operator != (const State& lhs, const State& rhs);
std::ostream& operator << (std::ostream& os, const StateType& type);
std::ostream& operator << (std::ostream& os, const State& state);

struct Token const * GetNextToken(struct TokenStreamReader* reader);
struct Token const * SeekNextToken(struct TokenStreamReader* reader, size_t next);

TokenStreamReader CreateTokenStreamReader(const std::vector<Token>& tokens);

bool operator == (const Token& lhs, const Token& rhs);
bool operator != (const Token& lhs, const Token& rhs);
std::ostream& operator << (std::ostream& os, const TokenType& tokenType);
std::ostream& operator << (std::ostream& os, const Token& token);

Token ReadToken(const std::string& str);
Token CreateToken(TokenType tokenType, const std::string& str);
std::vector<Token> ReadAllTokens(const std::string& str);

Variables CreateVariables(const std::vector<std::string>& variables);
bool operator == (const Variables& lhs, const Variables& rhs);
bool operator != (const Variables& lhs, const Variables& rhs);
std::ostream& operator << (std::ostream& os, const Variables& varibales);

function_call_id_t DefineFunctionCall(const std::string& name, std::vector<object_id_t> args);
function_call_id_t DefineFunctionCall(std::vector<object_id_t> args);

bool operator == (const Object& lhs, const Object& rhs);
bool operator != (const Object& lhs, const Object& rhs);
std::ostream& operator << (std::ostream& os, const ObjectType& type);
std::ostream& operator << (std::ostream& os, const Object& object);

Object CreateInteger(int32_t value);

std::ostream& operator << (std::ostream& os, const ExpressionType& type);