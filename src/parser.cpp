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
    case TokenTypes::LineComment:
        return "Line Comment";
    case TokenTypes::BlockComment:
        return "Block Comment";
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
    if (c == '\n' || c == '\r') {
        m_row++;
        m_col = 1;
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

json_parser::Token *json_parser::Parser::match_token(TokenTypes type, bool required) {
    Token *current = current_token();
    if (current->m_type == type) {
        next_token();
        return current;
    }

    if (required) {
        error(
            "Expected token: " +
                token_type_to_string(type) +
            "\nActual token: " +
                token_type_to_string(current->m_type),
            current->m_row,
            current->m_col
        );
    }

    return nullptr;
}

json_parser::JsonArray *json_parser::Parser::parse_array() {
    Token *token = match_token(TokenTypes::OpenBracket, true);

    JsonArray *result = new JsonArray(token->m_pos);
    add_comments(result);

    while (current_token()->m_type != TokenTypes::CloseBracket) {
        result->add_child(parse_object());
        match_token(TokenTypes::Comma);
    }

    match_token(TokenTypes::CloseBracket, true);

    return result;
}
json_parser::JsonObj *json_parser::Parser::parse_object() {
    Token *token = match_token(TokenTypes::OpenBrace, true);

    JsonObj *ret = new JsonObj(token->m_pos);
    add_comments(ret);
    
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

        ret->set(key, result);

        match_token(TokenTypes::Comma);

        token = current_token();
    }

    match_token(TokenTypes::CloseBrace, true);

    return ret;
}
json_parser::JsonString *json_parser::Parser::parse_string() {
    Token *token = match_token(TokenTypes::String, true);
    JsonString *result = new JsonString(token->m_pos, token->m_value);
    add_comments(result);
    return result;
}
json_parser::JsonBool *json_parser::Parser::parse_bool() {
    Token *boolToken = match_token(TokenTypes::Bool, true);
    JsonBool *result = new JsonBool(boolToken->m_pos, boolToken->m_value == "true");
    add_comments(result);
    return result;
}
json_parser::JsonNull *json_parser::Parser::parse_null() {
    Token *token = match_token(TokenTypes::Null, true);
    JsonNull *result = new JsonNull(token->m_pos);
    add_comments(result);
    return result;
}
json_parser::JsonNumber *json_parser::Parser::parse_number() {
    Token *numberToken = match_token(TokenTypes::Number, true);
    JsonNumber *result = new JsonNumber(numberToken->m_pos, std::stod(numberToken->m_value));
    add_comments(result);
    return result;
}

void json_parser::Parser::add_comments(Object *ptr) {
    for (size_t i = 0; i < m_comments.size(); i++) {
        if (m_comments[i]->m_pos <= ptr->m_pos) {
            Object *comment = nullptr;
            if (m_comments[i]->m_type == TokenTypes::BlockComment) {
                comment = new JsonBlockComment(m_comments[i]->m_pos, m_comments[i]->m_value);
            } else {
                comment = new JsonLineComment(m_comments[i]->m_pos, m_comments[i]->m_value);
            }
            ptr->add_comment(comment, true);
            m_comments.erase(m_comments.begin()+i);
            i--;
        }
    }
}
void json_parser::Parser::error(std::string msg, size_t line, size_t col) {
    throw std::runtime_error(
        "Fatal error at: line " +
            std::to_string(line) +
        ", col " +
            std::to_string(col) +
        ":\n" + msg
    );
}

json_parser::Token *json_parser::Parser::get_next_token() {
    size_t start = m_pos;
    size_t startRow = m_row;
    size_t startCol = m_col;
    char startChr = current_chr();

    // misc: eof token
    if (eof()) {
        next_chr();
        return new Token(TokenTypes::EOFToken, current_chr(), start, startRow, startCol);
    }

    // misc: whitespace token
    if (isspace(current_chr())) {
        while (!eof() && isspace(current_chr()))
            next_chr();
        size_t len = m_pos - start;
        std::string whitespace = m_str.substr(start, m_pos - start);
        return new Token(TokenTypes::WhiteSpace, whitespace, start, startRow, startCol);
    }

    // comments
    if (current_chr() == '/') {
        // comments: line comment
        next_chr();
        if (current_chr() == '/') {
            next_chr();
            while (!eof() && current_chr() != '\n' && current_chr() != '\r')
                next_chr();
            // dont include the // slashes
            std::string comment = m_str.substr(start+2, m_pos-start-2);
            return new Token(TokenTypes::LineComment, comment, start, startRow, startCol);
        }
        
        // comments: block comment
        else if (current_chr() == '*') {
            bool hasClosingTag = false;

            next_chr();
            while (!eof()) {
                // if there is space for closing */
                if (m_pos+1 < m_str.length() &&
                    m_str.substr(m_pos, 2) == "*/") {
                    next_chr();
                    next_chr();
                    hasClosingTag = true;
                    break;
                }
                next_chr();
            }
            
            // dont include the // slashes
            std::string comment = m_str.substr(start+2, m_pos - hasClosingTag*2 - start - 2);
            return new Token(TokenTypes::BlockComment, comment, start, startRow, startCol);
        }
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
                    error("Unicode is not supported", m_row, m_col);
                default:
                    error("Unrecognised escape sequence", m_row, m_col);
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
            next_chr();
            return new Token(TokenTypes::String, str, start, startRow, startCol);
        }

        // no closing quote
        error("No closing quote for string", m_row, m_col);
    }
    
    // types: number
    if (isdigit(current_chr()) || current_chr() == '-') {
        // advance so that the - is not considered
        next_chr();

        // integer
        while (!eof() && isdigit(current_chr()))
            next_chr();

        // fraction
        if (current_chr() == '.') {
            next_chr();

            // decimal places
            if (isdigit(current_chr())) {
                while (!eof() && isdigit(current_chr()))
                    next_chr();
            } else {
                error("A decimal point must be followed by digits", m_row, m_col);
            }
        }

        // exponent
        if (tolower(current_chr()) == 'e') {
            next_chr();
            if (current_chr() == '-'||current_chr()=='+')
                next_chr();
            if (isdigit(current_chr())) {
                while (!eof() && isdigit(current_chr()))
                    next_chr();
            } else {
                error("Exponent must be followed by an integer", m_row, m_col);
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
    error(std::string("Bad Token: ") + startChr, m_row, m_col);
    next_chr();
    return new Token(TokenTypes::BadToken, startChr, start, startRow, startCol);
}

json_parser::Parser::Parser(const std::string &str) {
    m_str = str;
    m_pos = 0;
    m_row = 1;
    m_col = 1;
    m_token_pos = 0;

    while (1) {
        Token *token = get_next_token();

        if (token->m_type != TokenTypes::WhiteSpace &&
            token->m_type != TokenTypes::BadToken) {
                if (token->m_type == TokenTypes::BlockComment ||
                    token->m_type == TokenTypes::LineComment) {

                    m_comments.push_back(token);
                } else {
                    m_tokens.push_back(token);
                }

                if (token->m_type == TokenTypes::EOFToken)
                    break;
        }
    }
}
json_parser::Parser::~Parser() {
    for (Token *token : m_tokens)
        delete token;
    for (Token *token : m_comments)
        delete token;
}

json_parser::JsonObj *json_parser::Parser::parse() {
    JsonObj *result = parse_object();

    // add any remaining comments to the last jsonObj
    for (Token *token : m_comments) {
        Object *comment = nullptr;
        if (token->m_type == TokenTypes::BlockComment) {
            comment = new JsonBlockComment(token->m_pos, token->m_value);
        } else {
            comment = new JsonLineComment(token->m_pos, token->m_value);
        }
        result->add_comment(comment, false);
    }

    return result;
}
