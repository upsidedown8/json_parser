#include "json_parser.hpp"

json_parser::JsonDoc::JsonDoc(const std::string &data) {
    Parser parser(data);
    root = parser.parse();
}
json_parser::JsonDoc::~JsonDoc() {
    delete root;
}

std::string json_parser::JsonDoc::to_string() {
    return root->to_string_comments("");
}
