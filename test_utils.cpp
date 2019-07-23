#include "test_utils.h"

#include <tuple>
#include <cstring>

extern "C" {
#include "function_pool.h"
}

State CreateState(uint16_t level, StateType type) {
    State state;
    state.level = level;
    state.type = type;

    return state;
}

State GetInvalidState() {
    return CreateState(0, StateType::INVALID_STATE);
}

bool operator == (const State& lhs, const State& rhs) {
    return std::tie(lhs.type, lhs.level) == std::tie(rhs.type, rhs.level);
}

bool operator != (const State& lhs, const State& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator << (std::ostream& os, const StateType& type) {
    switch (type) {
#define CASE(X) case X: os << #X; break;
        FOR_EACH_STATE(CASE)
#undef CASE
        default:
            os << "UnknownStateType";
    }
    return os;
}

std::ostream& operator << (std::ostream& os, const State& state) {
    os << "State(" << state.type << ", " << state.level << ")";
    return os;
}

Token const * GetReaderToken(TokenStreamReader* reader, size_t position) {
    return position < reader->end ? &reader->buffer[position] : NULL;
}

Token const * GetNextToken(TokenStreamReader* reader) {
    return GetReaderToken(reader, reader->begin++);
}

Token const * SeekNextToken(TokenStreamReader* reader, size_t next) {
    return GetReaderToken(reader, reader->begin + next);
}

TokenStreamReader CreateTokenStreamReader(const std::vector<Token>& tokens) {
    TokenStreamReader reader{};
    reader.get = GetNextToken;
    reader.seek = SeekNextToken;
    reader.begin = 0;
    reader.end = tokens.size();
    std::copy(tokens.begin(), tokens.end(), reader.buffer);
    return reader;
}

Variables CreateVariables(const std::vector<std::string>& variables) {
    Variables result{};
    result.argc = static_cast<arguments_size_t>(variables.size());
    for (size_t i = 0; i < variables.size(); ++i) {
        std::copy(variables[i].begin(), variables[i].end(), result.variables[i]);
    }
    return result;
}

bool operator == (const Variables& lhs, const Variables& rhs) {
    if (lhs.argc != rhs.argc)
        return false;
    for (arguments_size_t i = 0; i < lhs.argc; ++i) {
        if (strcmp(lhs.variables[i], rhs.variables[i]) != 0)
            return false;
    }
    return true;
}

bool operator != (const Variables& lhs, const Variables& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator << (std::ostream& os, const Variables& varibales) {
    os << "{";
    for (arguments_size_t i = 0; i < varibales.argc; ++i) {
        os << "(" << i << ": " << varibales.variables[i] << ")";
        os << (i + 1 != varibales.argc ? ", " : "}");
    }
    return os;
}

bool operator == (const Token& lhs, const Token& rhs) {
    return lhs.type == rhs.type && strcmp(lhs.string, rhs.string) == 0;
}

bool operator != (const Token& lhs, const Token& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator << (std::ostream& os, const TokenType& tokenType) {
    switch (tokenType) {
#define CASE(X) case X: os << #X; break;
        FOR_EACH_TOKEN_TYPE(CASE)
#undef CASE
        default:
            os << "UnknownTokenType";
    }
    return os;
}

std::ostream& operator << (std::ostream& os, const Token& token) {
    os << "Token(" << token.type << ", " << token.string << ")";
    return os;
}

Token ReadToken(const std::string& str) {
    Token result;
    read_token(&result, str.c_str());
    return result;
}

Token CreateToken(TokenType tokenType, const std::string& str) {
    Token result;
    result.type = tokenType;
    strcpy(result.string, str.c_str());

    return result;
}

std::vector<Token> ReadAllTokens(const std::string& str) {
    Token tokens[40];

    auto next = [](Token* x) { return x + 1; };
    auto read = read_all_tokens(tokens, next, str.c_str());

    return std::vector<Token>(tokens, tokens + read);
}

function_call_id_t DefineFunctionCall(const std::string& name, std::vector<object_id_t> args) {
    auto id = add_function_call();
    auto call = get_function_call_by_id(id);
    call->arguments[0] = create_function_object(get_function(name.c_str())->id);
    call->argc = static_cast<arguments_size_t>(args.size() + 1);
    for (arguments_size_t i = 1; i <= args.size(); ++i)
        call->arguments[i] = args[i - 1];
    return id;
}


function_call_id_t DefineFunctionCall(std::vector<object_id_t> args) {
    auto id = add_function_call();
    auto call = get_function_call_by_id(id);
    call->argc = static_cast<arguments_size_t>(args.size());
    for (arguments_size_t i = 0; i < args.size(); ++i)
        call->arguments[i] = args[i];
    return id;
}

bool operator == (const Pair& lhs, const Pair& rhs) {
#define GET_ELEM(ARG, NAME) *get_object_by_id((ARG).NAME)
    return std::tie(GET_ELEM(lhs, first), GET_ELEM(lhs, second))
                == std::tie(GET_ELEM(rhs, first), GET_ELEM(rhs, second));
#undef GET_ELEM
}

bool operator == (const FunctionCall& lhs, const FunctionCall& rhs) {
    if (lhs.argc != rhs.argc)
        return false;

    for (size_t i = 0; i < lhs.argc; ++i) {
        if (*get_object_by_id(lhs.arguments[i]) != *get_object_by_id(rhs.arguments[i]))
            return false;
    }
    return true;
}

bool operator == (const Function& lhs, const Function& rhs) {
    if (lhs.type != rhs.type)
        return false;
    if (lhs.arguments_amount != rhs.arguments_amount)
        return false;
    switch (lhs.type) {
        case SIMPLE_FUNCTION:
            return lhs.call == rhs.call;
        case COMPOSITE_FUNCTION:
            return *get_object_by_id(lhs.object_id) == *get_object_by_id(rhs.object_id);
    }
    return false;
}

#define IS_EQUAL(TYPE) is_equal_ ## TYPE
#define DEFINE_IS_EQUAL(TYPE) bool IS_EQUAL(TYPE) (const Object& lhs, const Object& rhs)

#define DEFINE_EQUAL_BY_KEY(TYPE, KEY)                                              \
DEFINE_IS_EQUAL(TYPE) {                                                             \
    return KEY(lhs.OBJECT_FIELD(TYPE)) == KEY(rhs.OBJECT_FIELD(TYPE));              \
}

#define DEFINE_EQUAL_BY_FIELD(TYPE)                             \
DEFINE_IS_EQUAL(TYPE) {                                         \
    return lhs.OBJECT_FIELD(TYPE) == rhs.OBJECT_FIELD(TYPE);    \
}

DEFINE_EQUAL_BY_FIELD(INTEGER_TYPE)
DEFINE_EQUAL_BY_FIELD(NULL_TYPE)
DEFINE_EQUAL_BY_FIELD(FUNCTION_ARGUMENT_TYPE)
DEFINE_EQUAL_BY_FIELD(EXCEPTION_TYPE)
DEFINE_EQUAL_BY_FIELD(PAIR_TYPE)

DEFINE_EQUAL_BY_KEY(FUNCTION_CALL_TYPE, *get_function_call_by_id)
DEFINE_EQUAL_BY_KEY(FUNCTION_TYPE, *get_function_by_id)

bool operator ==(const Object& lhs, const Object& rhs) {
    if (lhs.type != rhs.type)
        return false;
    switch (lhs.type) {
#define CASE(TYPE) case TYPE: return IS_EQUAL(TYPE)(lhs, rhs);
        FOR_EACH_OBJECT_TYPE(CASE)
#undef CASE
    }
    return false;
}

bool operator !=(const Object& lhs, const Object& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator <<(std::ostream& os, const ObjectType& type) {
    switch (type) {
#define CASE(TYPE) case TYPE: os << #TYPE; break;
        FOR_EACH_OBJECT_TYPE(CASE)
#undef CASE
        default:
            os << "UnknownObjectType";
    }
    return os;
}

std::ostream& operator << (std::ostream& os, const Pair& pair) {
    os << "{" << pair.first << ", " << pair.second << "}";
    return os;
}

std::ostream& operator << (std::ostream& os, arguments_size_t value) {
    os << (int32_t) value;
    return os;
}

std::ostream& operator << (std::ostream& os, const Object& object) {
    os << "Object(" << object.type << ", ";
    switch (object.type) {
#define CASE(TYPE) case TYPE: os << object.OBJECT_FIELD(TYPE); break;
        FOR_EACH_OBJECT_TYPE(CASE)
#undef CASE
        default:
            os << "UnknownValue";
    }
    os << ")";
    return os;
}

Object CreateInteger(int32_t value) {
    Object result;
    result.type = ObjectType::INTEGER_TYPE;
    result.OBJECT_FIELD(INTEGER_TYPE) = value;
    return result;
}

std::ostream& operator <<(std::ostream& os, const ExpressionType& type) {
    switch (type) {
#define CASE(TYPE) case TYPE: os << #TYPE; break;
    FOR_EACH_EXPRESSION_TYPE(CASE)
#undef CASE
        default:
            os << "UnknownExpressionType";
    }
    return os;
}
