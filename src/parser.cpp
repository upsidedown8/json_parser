#include "parser.hpp"

#include <stdexcept>
#include <string>

/* Utils */
std::string json_parser::token_type_to_string(TokenTypes type) {
    switch (type) {
    case TokenTypes::String:
        return "String";
    case TokenTypes::Number:
        return "Number";
    case TokenTypes::Bool:
        return "Bool";
    case TokenTypes::Null:
        return "Null";
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
json_parser::Token::Token(TokenTypes type, char value, size_t pos, size_t row, size_t col)
    : Token(type, std::string(1, value), pos, row, col) {}
json_parser::Token::Token(TokenTypes type, const std::string &value, size_t pos, size_t row, size_t col) {
    m_type = type;
    m_value = value;
    m_pos = pos;
    m_row = row;
    m_col = col;
}

std::string json_parser::Token::to_string() {
    return 
        "row: " + std::to_string(m_row) +
        " col: " + std::to_string(m_col) +
        " type: " + token_type_to_string(m_type) +
        " value: " + m_value;
}

/* Parser */
char json_parser::Parser::current_chr() {
    return m_str[m_pos];
}
char json_parser::Parser::next_chr() {
    m_pos++;
    m_col++;
    if (eof())
        return '\0';
    char c = m_str[m_pos];
    if (c == '\n') {
        m_row++;
        m_col = 0;
    }
    return c;
}

bool json_parser::Parser::eof() {
    return m_pos >= m_str.length();
}

json_parser::Token *json_parser::Parser::current_token() {
    return m_tokens[std::min(m_token_pos, m_tokens.size()-1)];
}
json_parser::Token *json_parser::Parser::next_token() {
    return m_tokens[std::min(++m_token_pos, m_tokens.size()-1)];
}

json_parser::Token *json_parser::Parser::match_token(TokenTypes type) {
    Token *current = current_token();
    if (current->m_type == type) {
        next_token();
        return current;
    }
    return nullptr;
}

json_parser::JsonArray *json_parser::Parser::parse_array() {
    JsonArray *result = new JsonArray;

    match_token(TokenTypes::OpenBracket);
    
    while (current_token()->m_type != TokenTypes::CloseBracket) {
        result->add_child(parse_object());
        match_token(TokenTypes::Comma);
    }

    match_token(TokenTypes::CloseBracket);

    return result;
}
json_parser::JsonObj *json_parser::Parser::parse_object() {
    JsonObj *ret = new JsonObj;
    
    Token *token = match_token(TokenTypes::OpenBrace);

    while (token->m_type != TokenTypes::EOFToken &&
           token->m_type != TokenTypes::CloseBrace) {
        JsonString *key = parse_string();
        match_token(TokenTypes::Colon);
        
        Object *result = nullptr;

        switch (current_token()->m_type) {
        case TokenTypes::OpenBrace:
            result = parse_object();
            break;
        case TokenTypes::OpenBracket:
            result = parse_array();
            break;
        case TokenTypes::Bool:
            result = parse_bool();
            break;
        case TokenTypes::Number:
            result = parse_number();
            break;
        case TokenTypes::String:
            result = parse_string();
            break;
        case TokenTypes::Null:
        default:
            result = parse_null();
            break;
        }

        ret->set(key->m_val, result);

        match_token(TokenTypes::Comma);

        token = current_token();
    }

    match_token(TokenTypes::CloseBrace);

    return ret;
}
json_parser::JsonString *json_parser::Parser::parse_string() {
    return new JsonString(match_token(TokenTypes::String)->m_value);
}
json_parser::JsonBool *json_parser::Parser::parse_bool() {
    Token *boolToken = match_token(TokenTypes::Bool);
    return new JsonBool(boolToken->m_value == "true");
}
json_parser::JsonNull *json_parser::Parser::parse_null() {
    match_token(TokenTypes::Null);
    return new JsonNull;
}
json_parser::JsonNumber *json_parser::Parser::parse_number() {
    Token *numberToken = match_token(TokenTypes::Number);
    return new JsonNumber(std::stod(numberToken->m_value));
}

json_parser::Token *json_parser::Parser::get_next_token() {
    size_t start = m_pos;
    size_t startRow = m_row;
    size_t startCol = m_col;
    char startChr = current_chr();

    // misc: eof token
    if (m_pos >= m_str.length()) {
        next_chr();
        return new Token(TokenTypes::EOFToken, current_chr(), start, startRow, startCol);
    }

    // misc: whitespace token
    if (isspace(current_chr())) {
        while (isspace(current_chr()))
            next_chr();
        size_t len = m_pos - start;
        std::string whitespace = m_str.substr(start, m_pos - start);
        return new Token(TokenTypes::WhiteSpace, whitespace, start, startRow, startCol);
    }
    
    // objects: open & close braces and colons
    if (current_chr() == '{') {
        next_chr();
        return new Token(TokenTypes::OpenBrace, startChr, start, startRow, startCol);
    }
    if (current_chr() == '}') {
        next_chr();
        return new Token(TokenTypes::CloseBrace, startChr, start, startRow, startCol);
    }
    if (current_chr() == ':') {
        next_chr();
        return new Token(TokenTypes::Colon, startChr, start, startRow, startCol);
    }

    // arrays: open & close brackets and commas
    if (current_chr() == '[') {
        next_chr();
        return new Token(TokenTypes::OpenBracket, startChr, start, startRow, startCol);
    }
    if (current_chr() == ']') {
        next_chr();
        return new Token(TokenTypes::CloseBracket, startChr, start, startRow, startCol);
    }
    if (current_chr() == ',') {
        next_chr();
        return new Token(TokenTypes::Comma, startChr, start, startRow, startCol);
    }

    // types: string
    if (current_chr() == '"') {
        bool escape = false;
        std::string str;
        // loop till eof
        while (!eof()) {
            next_chr();
            if (escape) {
                switch (current_chr()) {
                case '\\': str += '\\'; break;
                case '/': str += '/'; break;
                case 'b': str += '\b'; break;
                case 'f': str += '\f'; break;
                case 'n': str += '\n'; break;
                case 'r': str += '\r'; break;
                case 't': str += '\t'; break;
                case 'u':
                    throw std::runtime_error("Unicode is not supported");
                default:
                    throw std::runtime_error("Unrecognised escape sequence");
                }
                escape = false;
            } else if (current_chr() == '\\') {
                str += current_chr();
                escape = true;
            } else if (current_chr() == '"') {
                break;
            } else {
                str += current_chr();
            }
        }

        // reached end of string
        if (current_chr() == '"') {
            m_pos++;
            m_col++;
            return new Token(TokenTypes::String, str, start, startRow, startCol);
        }

        // no closing quote
        throw std::runtime_error("No closing quote for string");
    }
    
    // types: number
    if (isdigit(current_chr()) || current_chr() == '-') {
        // advance so that the - is not considered
        next_chr();

        // integer
        while (isdigit(current_chr()))
            next_chr();

        // fraction
        if (current_chr() == '.') {
            next_chr();

            // decimal places
            if (isdigit(current_chr())) {
                while (isdigit(current_chr()))
                    next_chr();
            } else {
                throw std::runtime_error("A decimal point must be followed by digits");
            }
        }

        // exponent
        if (tolower(current_chr()) == 'e') {
            next_chr();
            if (current_chr() == '-'||current_chr()=='+')
                next_chr();
            if (isdigit(current_chr())) {
                while (isdigit(current_chr()))
                    next_chr();
            } else {
                throw std::runtime_error("Exponent must be followed by an integer");
            }
        }

        std::string str = m_str.substr(start, m_pos - start);
        return new Token(TokenTypes::Number, str, start, startRow, startCol);
    }

    // check there is space for word
    if (m_pos+3 < m_str.length()) {
        if (m_pos+4 < m_str.length()) {
            std::string str = m_str.substr(start, 5);

            if (str == "false") {
                m_pos += 5;
                m_col += 5;
                return new Token(TokenTypes::Bool, str, m_pos, startRow, startCol);
            }
        }
        
        std::string str = m_str.substr(start, 4);
        // types: bool (true)
        if (str == "true") {
            m_pos += 4;
            m_col += 4;
            return new Token(TokenTypes::Bool, str, m_pos, startRow, startCol);
        }
        // types: null
        if (str == "null") {
            m_pos += 4;
            m_col += 4;
            return new Token(TokenTypes::Null, str, m_pos, startRow, startCol);
        }
    }

    // misc: EOF token
    next_chr();
    return new Token(TokenTypes::BadToken, startChr, start, startRow, startCol);
}

json_parser::Parser::Parser(const std::string &str) {
    m_str = str;
    m_pos = 0;
    m_row = 0;
    m_col = 0;
    m_token_pos = 0;

    while (1) {
        Token *token = get_next_token();

        if (token->m_type != TokenTypes::WhiteSpace &&
            token->m_type != TokenTypes::BadToken) {
                m_tokens.push_back(token);

                if (token->m_type == TokenTypes::EOFToken)
                    break;
        }
    }
}
json_parser::Parser::~Parser() {
    for (Token *token : m_tokens)
        delete token;
}

json_parser::JsonObj *json_parser::Parser::parse() {
    return parse_object();
}
