#if !defined(JSONPARSER_HPP)
#define JSONPARSER_HPP

#include <istream>

namespace json_parser {

enum class TokenTypes {
    // types
    String,
    Number,
    Object,
    Array,
    Bool,
    Null,

    // strings
    SingleQuote,  // '
    DoubleQuote,  // "

    // objects
    OpenBrace,    // {
    CloseBrace,   // }
    Colon,        // :

    // arrays
    OpenBracket,  // [
    CloseBracket, // ]
    Comma,        // ,

    // misc
    EOFToken,
    WhiteSpace,
    BadToken
};

std::string type_to_string(TokenTypes type);

struct Token {
    TokenTypes m_type;
    std::string m_value;
    size_t m_row, m_col;

    Token(TokenTypes type, char value, size_t row, size_t col);
    Token(TokenTypes type, const std::string &value, size_t row, size_t col);
};

class Parser {
private:
    std::string m_str;
    size_t m_pos, m_row, m_col;

    char current_chr();
    char next_chr();
    char peek_chr();

    int row();
    int col();

public:
    Parser(const std::string &str);

    Token next_token();

};

class JsonDoc {

};

}

#endif // JSONPARSER_HPP
