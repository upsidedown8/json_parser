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
    JsonNull
};

std::string obj_type_to_string(JsonTypes type);

struct Object {
    virtual JsonTypes get_type() = 0;
    virtual std::string to_string(const std::string &indent) = 0;
};

struct JsonArray : public virtual Object {
private:
    std::vector<Object*> m_children;

public:
    ~JsonArray();

    void add_child(Object *child);

    size_t size();

    Object *operator[](size_t idx);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonObj : public virtual Object {
private:
    std::map<std::string, Object*> m_values;

public:
    ~JsonObj();

    Object *operator[](const std::string &key);

    Object *get(const std::string &key);
    void set(const std::string &key, Object *val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonString : public virtual Object {
    std::string m_val;

    JsonString(const std::string &val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonNumber : public virtual Object {
    double m_val;

    JsonNumber(double val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonBool : public virtual Object {
    bool m_val;

    JsonBool(bool val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

struct JsonNull : public virtual Object {
    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

}

#endif // JSONPARSER_AST_HPP
