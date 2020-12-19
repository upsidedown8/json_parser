#if !defined(JSONPARSER_AST_HPP)
#define JSONPARSER_AST_HPP

#include <string>
#include <vector>
#include <map>

namespace json_parser {

enum class JsonTypes {
    JsonObj,
    JsonArray,

    JsonString,
    JsonNumber,
    JsonBool,
    JsonNull,

    JsonLineComment,
    JsonBlockComment
};

std::string obj_type_to_string(JsonTypes type);

class Token;

struct Object {
    size_t m_pos;

    Object(size_t pos);
    virtual ~Object(){}

    virtual JsonTypes get_type() = 0;
    virtual std::string to_string(const std::string &indent) = 0;
};

struct JsonArray : public virtual Object {
private:
    std::vector<Object*> m_children;

public:
    JsonArray(size_t pos);
    ~JsonArray();

    void add_child(Object *child);

    size_t size();

    Object *operator[](size_t idx);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonObj : public virtual Object {
private:
    std::vector<std::string> m_keys;
    std::map<std::string, Object*> m_values;

public:
    JsonObj(size_t pos);
    ~JsonObj();

    Object *operator[](const std::string &key);

    Object *get(const std::string &key);
    void set(const std::string &key, Object *val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonString : public virtual Object {
    std::string m_val;

    JsonString(size_t pos, const std::string &val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonNumber : public virtual Object {
    double m_val;

    JsonNumber(size_t pos, double val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonBool : public virtual Object {
    bool m_val;

    JsonBool(size_t pos, bool val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonNull : public virtual Object {
    JsonNull(size_t pos);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonLineComment : public virtual Object {
    std::string m_val;

    JsonLineComment(size_t pos, const std::string &comment);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonBlockComment : public virtual Object {
    std::string m_val;

    JsonBlockComment(size_t pos, const std::string &comment);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

}

#endif // JSONPARSER_AST_HPP
