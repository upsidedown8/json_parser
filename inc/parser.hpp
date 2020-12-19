#if !defined(JSONPARSER_PARSER_HPP)
#define JSONPARSER_PARSER_HPP

#include <string>
#include "ast.hpp"

namespace json_parser {

enum class TokenTypes {
    // types
    String,
    Number,
    Bool,
    Null,

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

std::string token_type_to_string(TokenTypes type);

struct Token {
    TokenTypes m_type;
    std::string m_value;
    size_t m_row, m_col, m_pos;

    Token(TokenTypes type, char value, size_t pos, size_t row, size_t col);
    Token(TokenTypes type, const std::string &value, size_t pos, size_t row, size_t col);

    std::string to_string();
};

class Parser {
private:
    // lexer
    std::string m_str;
    size_t m_pos, m_row, m_col;

    char current_chr();
    char next_chr();
    bool eof();

    Token *get_next_token();

    // parser
    size_t m_token_pos;
    std::vector<Token*> m_tokens;
    Token *current_token();
    Token *next_token();

    Token *match_token(TokenTypes type);
    
    JsonArray *parse_array();
    JsonObj *parse_object();

    JsonString *parse_string();
    JsonBool *parse_bool();
    JsonNull *parse_null();
    JsonNumber *parse_number();
    
public:
    Parser(const std::string &str);
    ~Parser();

    JsonObj *parse();
};

}

#endif // JSONPARSER_PARSER_HPP
