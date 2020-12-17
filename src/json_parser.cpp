#include "json_parser.hpp"
#include <stdexcept>

/* Utils */
std::string json_parser::type_to_string(TokenTypes type) {
    switch (type) {
    case TokenTypes::Number:
        return "Number";
    case TokenTypes::Object:
        return "Object";
    case TokenTypes::Array:
        return "Array";
    case TokenTypes::Bool:
        return "Bool";
    case TokenTypes::Null:
        return "Null";
    case TokenTypes::SingleQuote:
        return "Single Quote";
    case TokenTypes::DoubleQuote:
        return "Double Quote";
    case TokenTypes::OpenBrace:
        return "Open Brace";
    case TokenTypes::CloseBrace:
        return "Close Brace";
    case TokenTypes::Colon:
        return "Colon";
    case TokenTypes::OpenBracket:
        return "Open Bracket";
    case TokenTypes::CloseBracket:
        return "Close Bracket";
    case TokenTypes::Comma:
        return "Comma";
    case TokenTypes::EOFToken:
        return "EOF Token";
    case TokenTypes::WhiteSpace:
        return "White Space";
    case TokenTypes::BadToken:
        return "Bad Token";
    default:
        throw std::runtime_error("Unrecognised TokenType: " + std::to_string((int)type));
    }
}

/* Token */
json_parser::Token::Token(TokenTypes type, char value, size_t row, size_t col)
    : Token(type, std::string(1, value), row, col) {}
json_parser::Token::Token(TokenTypes type, const std::string &value, size_t row, size_t col) {
    m_type = type;
    m_value = value;
    m_row = row;
    m_col = col;
}

/* Parser */
char json_parser::Parser::current_chr() {
    return m_str[m_pos];
}
char json_parser::Parser::next_chr() {
    m_pos++;
    m_col++;
    if (m_pos >= m_str.length())
        return '\0';
    char c = m_str[m_pos];
    if (c == '\n') {
        m_row++;
        m_col = 0;
    }
    return c;
}
char json_parser::Parser::peek_chr() {
    if (m_pos+1 >= m_str.length())
        return '\0';
    return m_str[m_pos+1];
}

int json_parser::Parser::row() {
    return m_row;
}
int json_parser::Parser::col() {
    return m_col;
}

json_parser::Parser::Parser(const std::string &str) {
    m_str = str;
    m_pos = 0;
    m_row = 0;
    m_col = 0;
}

json_parser::Token json_parser::Parser::next_token() {
    int row = row();
    int col = col();
    char current = current_chr();
    int start = m_pos++;

    if (m_pos >= m_str.length())
        return Token(TokenTypes::EOFToken, current, row, col);
    if (isspace(current_chr())) {
        while (isspace(current_chr()))
            next_chr();
    }

    // open and close braces
    if (current == '{')
        return Token(TokenTypes::OpenBrace, current, row, col);
    if (current == '}')
        return Token(TokenTypes::CloseBrace, current, row, col);

    // open and close brackets
    if (current == '{')
        return Token(TokenTypes::OpenBrace, current, row, col);
    if (current == '}')
        return Token(TokenTypes::CloseBrace, current, row, col);
    


    return Token(TokenTypes::BadToken, current, row, col);
}

