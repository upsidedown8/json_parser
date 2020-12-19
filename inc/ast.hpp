#if !defined(JSONPARSER_AST_HPP)
#define JSONPARSER_AST_HPP

#include <string>
#include <vector>

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

class Object {
public:
    size_t m_pos;
    std::vector<Object*> m_comment_before;
    std::vector<Object*> m_comment_after;

    Object(size_t pos);
    virtual ~Object(){}

    void add_comment(Object *comment, bool before);

    std::string to_string_comments(const std::string &indent);

    virtual std::string to_string(const std::string &indent) = 0;
    virtual JsonTypes get_type() = 0;
};

class JsonString : public virtual Object {
public:
    std::string m_val;

    JsonString(size_t pos, const std::string &val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

class JsonNumber : public virtual Object {
public:
    double m_val;

    JsonNumber(size_t pos, double val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

class JsonBool : public virtual Object {
public:
    bool m_val;

    JsonBool(size_t pos, bool val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

class JsonNull : public virtual Object {
public:
    JsonNull(size_t pos);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

class JsonLineComment : public virtual Object {
public:
    std::string m_val;

    JsonLineComment(size_t pos, const std::string &comment);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

class JsonBlockComment : public virtual Object {
public:
    std::string m_val;

    JsonBlockComment(size_t pos, const std::string &comment);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

class JsonArray : public virtual Object {
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

class JsonObj : public virtual Object {
private:
    std::vector<std::string> m_keys;
    std::vector<Object*> m_keys_obj, m_values_obj;

public:
    JsonObj(size_t pos);
    ~JsonObj();

    Object *operator[](const std::string &key);

    Object *get(const std::string &key);
    void set(JsonString *key, Object *val);

    virtual JsonTypes get_type();
    virtual std::string to_string(const std::string &indent);
};

}

#endif // JSONPARSER_AST_HPP
