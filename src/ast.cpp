#include "ast.hpp"

/* Utils */
std::string json_parser::obj_type_to_string(JsonTypes type) {
    switch (type) {
        
    case JsonTypes::JsonObj: return "JsonObj";
    case JsonTypes::JsonArray: return "JsonArray";
    case JsonTypes::JsonString: return "JsonString";
    case JsonTypes::JsonNumber: return "JsonNumber";
    case JsonTypes::JsonBool: return "JsonBool";
    case JsonTypes::JsonNull: return "JsonNull";
    default: return "Unknown JsonTypes";
    
    }
}

/* JsonArray */
json_parser::JsonArray::~JsonArray() {
    for (Object *child : m_children)
        delete child;
}
void json_parser::JsonArray::add_child(Object *child) {
    m_children.push_back(child);
}
size_t json_parser::JsonArray::size() {
    return m_children.size();
}
json_parser::Object *json_parser::JsonArray::operator[](size_t idx) {
    return m_children[idx];
}
json_parser::JsonTypes json_parser::JsonArray::get_type() {
    return JsonTypes::JsonArray;
}
std::string json_parser::JsonArray::to_string(const std::string &indent) {
    std::string result = "[\n";

    for (int i = 0; i < m_children.size(); i++) {
        result += m_children[i]->to_string(indent + "    ");
        if (i + 1 < m_children.size())
            result += ',';
        result += '\n';
    }

    result += indent + ']';
    return result;
}

/* JsonObj */
json_parser::JsonObj::~JsonObj() {
    for (std::map<std::string, Object*>::iterator it = m_values.begin(); it != m_values.end(); it++) {
        delete it->second;
    }
}
json_parser::Object *json_parser::JsonObj::operator[](const std::string &key) {
    return m_values[key];
}
json_parser::Object *json_parser::JsonObj::get(const std::string &key) {
    return m_values[key];
}
void json_parser::JsonObj::set(const std::string &key, Object *val) {
    m_values[key] = val;
}
json_parser::JsonTypes json_parser::JsonObj::get_type() {
    return JsonTypes::JsonObj;
}
std::string json_parser::JsonObj::to_string(const std::string &indent) {
    std::string result = indent + "{\n";

    for (std::map<std::string, Object*>::iterator it = m_values.begin(); it != m_values.end(); it++) {
        result += indent + "    \""+it->first+"\": " + it->second->to_string(indent + "    ");
        if (std::distance(it, m_values.end()) > 1)
            result += ',';
        result += '\n';
    }

    result += indent + '}';
    return result;
}

/* JsonString */
json_parser::JsonString::JsonString(const std::string &val)
    : m_val(val) {}
json_parser::JsonTypes json_parser::JsonString::get_type() {
    return JsonTypes::JsonString;
}
std::string json_parser::JsonString::to_string(const std::string &indent) {
    return '"' + m_val + '"';
}

/* JsonNumber */
json_parser::JsonNumber::JsonNumber(double val)
    : m_val(val) {}
json_parser::JsonTypes json_parser::JsonNumber::get_type() {
    return JsonTypes::JsonNumber;
}
std::string json_parser::JsonNumber::to_string(const std::string &indent) {
    return std::to_string(m_val);
}

/* JsonBool */
json_parser::JsonBool::JsonBool(bool val)
    : m_val(val) {}
json_parser::JsonTypes json_parser::JsonBool::get_type() {
    return JsonTypes::JsonBool;
}
std::string json_parser::JsonBool::to_string(const std::string &indent) {
    return m_val ? "true" : "false";
}

/* JsonNull */
json_parser::JsonTypes json_parser::JsonNull::get_type() {
    return JsonTypes::JsonNull;
}
std::string json_parser::JsonNull::to_string(const std::string &indent) {
    return "null";
}
