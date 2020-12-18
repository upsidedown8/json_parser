#if !defined(JSONPARSER_HPP)
#define JSONPARSER_HPP

#include "ast.hpp"
#include "parser.hpp"

namespace json_parser {

class JsonDoc {
private:
    JsonObj *root = nullptr;

public:
    JsonDoc(const std::string &data);
    ~JsonDoc();

    std::string to_string();
};

}

#endif // JSONPARSER_HPP
