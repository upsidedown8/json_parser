#include "ast.hpp"

#include <algorithm>

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

/* Object */
json_parser::Object::Object(size_t pos)
    : m_pos(pos) {}
void json_parser::Object::add_comment(Object *comment, bool before) {
    (before ? m_comment_before : m_comment_after).push_back(comment);
}
std::string json_parser::Object::to_string_comments(const std::string &indent) {
    std::string result;
    for (int i = 0; i < m_comment_before.size(); i++) {
        result += m_comment_before[i]->to_string("") + indent;
    }
    result += to_string(indent);
    for (int i = 0; i < m_comment_after.size(); i++) {
        result += indent + m_comment_after[i]->to_string("");
    }
    return result;
}

/* JsonArray */
json_parser::JsonArray::JsonArray(size_t pos)
    : Object(pos) {}
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
    if (!m_children.size())
        return "[]";
    
    std::string result = "[\n";

    for (int i = 0; i < m_children.size(); i++) {
        result += indent + "    " + m_children[i]->to_string_comments(indent + "    ");
        if (i + 1 < m_children.size())
            result += ',';
        result += '\n';
    }

    result += indent + ']';
    return result;
}

/* JsonObj */
json_parser::JsonObj::JsonObj(size_t pos)
    : Object(pos) {}
json_parser::JsonObj::~JsonObj() {
    for (Object *obj : m_keys_obj)
        delete obj;
    for (Object *obj : m_values_obj)
        delete obj;
}
json_parser::Object *json_parser::JsonObj::operator[](const std::string &key) {
    return get(key);
}
json_parser::Object *json_parser::JsonObj::get(const std::string &key) {
    return m_values_obj[std::distance(m_keys.begin(), std::find(m_keys.begin(), m_keys.end(), key))];
}
void json_parser::JsonObj::set(JsonString *key, Object *val) {
    auto pos = std::find(m_keys.begin(), m_keys.end(), key->m_val);
    if (pos == m_keys.end()) {
        m_keys.push_back(key->m_val);
        m_keys_obj.push_back(key);
        m_values_obj.push_back(val);
    } else {
        size_t idx = std::distance(m_keys.begin(), pos);
        m_keys_obj[idx] = key;
        m_values_obj[idx] = val;
    }
}
json_parser::JsonTypes json_parser::JsonObj::get_type() {
    return JsonTypes::JsonObj;
}
std::string json_parser::JsonObj::to_string(const std::string &indent) {
    if (!m_keys.size())
        return "{}";

    std::string result = "{\n";

    // in order of keys to maintain input order
    for (size_t i = 0; i < m_keys.size(); i++) {
        result += 
            indent + "    " + m_keys_obj[i]->to_string_comments(indent + "    ") + ": " +
            m_values_obj[i]->to_string_comments(indent + "    ");
        if (i + 1 < m_keys.size())
            result += ',';
        result += '\n';
    }

    result += indent + '}';
    return result;
}

/* JsonString */
json_parser::JsonString::JsonString(size_t pos, const std::string &val)
    : Object(pos), m_val(val) {}
json_parser::JsonTypes json_parser::JsonString::get_type() {
    return JsonTypes::JsonString;
}
std::string json_parser::JsonString::to_string(const std::string &indent) {
    return '"' + m_val + '"';
}

/* JsonNumber */
json_parser::JsonNumber::JsonNumber(size_t pos, double val)
    : Object(pos), m_val(val) {}
json_parser::JsonTypes json_parser::JsonNumber::get_type() {
    return JsonTypes::JsonNumber;
}
std::string json_parser::JsonNumber::to_string(const std::string &indent) {
    return std::to_string(m_val);
}

/* JsonBool */
json_parser::JsonBool::JsonBool(size_t pos, bool val)
    : Object(pos), m_val(val) {}
json_parser::JsonTypes json_parser::JsonBool::get_type() {
    return JsonTypes::JsonBool;
}
std::string json_parser::JsonBool::to_string(const std::string &indent) {
    return m_val ? "true" : "false";
}

/* JsonNull */
json_parser::JsonNull::JsonNull(size_t pos)
    : Object(pos) {}
json_parser::JsonTypes json_parser::JsonNull::get_type() {
    return JsonTypes::JsonNull;
}
std::string json_parser::JsonNull::to_string(const std::string &indent) {
    return "null";
}

/* JsonLineComment */
json_parser::JsonLineComment::JsonLineComment(size_t pos, const std::string &comment)
    : Object(pos), m_val(comment) {}
json_parser::JsonTypes json_parser::JsonLineComment::get_type() {
    return JsonTypes::JsonLineComment;
}
std::string json_parser::JsonLineComment::to_string(const std::string &indent) {
    return "//" + m_val + '\n';
}

/* JsonBlockComment */
json_parser::JsonBlockComment::JsonBlockComment(size_t pos, const std::string &comment)
    : Object(pos), m_val(comment) {}
json_parser::JsonTypes json_parser::JsonBlockComment::get_type() {
    return JsonTypes::JsonBlockComment;
}
std::string json_parser::JsonBlockComment::to_string(const std::string &indent) {
    return "/*" + m_val + "*/\n";
}
