#include "json_parser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

// template<typename Parent>
// class JsonObjBuilder : public JsonObj {
// public:
//     JsonObjBuilder();

//     JsonNullBuilder &add_null(const std::string &key);
//     JsonNullBuilder &add_null(const std::string &key, JsonNull *obj);
    
//     JsonBoolBuilder &add_bool(const std::string &key);
//     JsonBoolBuilder &add_bool(const std::string &key, JsonBool *obj);
    
//     JsonStringBuilder &add_string(const std::string &key);
//     JsonStringBuilder &add_string(const std::string &key, JsonString *obj);
    
//     JsonNumberBuilder &add_number(const std::string &key);
//     JsonNumberBuilder &add_number(const std::string &key, JsonNumber *obj);

//     JsonArrayBuilder &add_array(const std::string &key);
//     JsonArrayBuilder &add_array(const std::string &key, JsonArray *obj);

//     Parent &done();
// };

// class JsonArrayBuilder : public JsonArray {
// public:
//     JsonArrayBuilder();

//     JsonArrayBuilder &add_object();
//     JsonArrayBuilder &add_object(JsonObj *object);

//     JsonArray *done();
// }

int main(int argc, char **argv) {
    std::string data, line;
    std::ifstream file(".vscode/launch.json");
    if (!file.is_open()) return -1;
    while (std::getline(file, line))
        data += line + '\n';
    file.close();

    try {
        json_parser::JsonDoc jsonDoc(data);

        std::cout << jsonDoc.to_string() << std::endl;
    } catch (std::runtime_error re) {
        std::cout << re.what() << std::endl;
    }

    // json_parser::JsonDoc::create()
    //     .add_bool("key1")
    //         .value(false)
    //         .block_comment_before("comment")
    //         .line_comment_before("comment")
    //         .block_comment_after("comment")
    //         .line_comment_after("comment")
    //         .done()
    //     .add_string("key2")
    //         .value(true)
    //         .done()
    //     .add_object("config")
    //         .add_string("name")
    //             .value("value")
    //             .done()
    //         .add_array(values)
    //         .add_array("items")
    //             .add_object()
    //                 .add<JsonBool>("isTrue")
    //                     .value(false)
    //                     .done()
    //                 .add<JsonBool>("isFalse")
    //                     .value(true)
    //                     .done()
    //                 .done()
    //             .add_object()
    //                 .add<JsonBool>("isTrue")
    //                     .value(false)
    //                     .done()
    //                 .add<JsonBool>("isFalse")
    //                     .value(true)
    //                     .done()
    //                 .done()
    //             .add_object()
    //                 .add<JsonBool>("isTrue")
    //                     .value(false)
    //                     .done()
    //                 .add<JsonBool>("isFalse")
    //                     .value(true)
    //                     .done()
    //                 .done()
    //             .done()
    //         .done()
    //     .done();

    return 0;
}
