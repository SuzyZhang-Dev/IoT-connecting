//
// Created by 张悦 on 20.4.2026.
//

#ifndef ASSIGNMENT3_JSONPARSER_H
#define ASSIGNMENT3_JSONPARSER_H
#include <string>
#include <map>

namespace JsonParser {
    class Value {
    public:
        Value()=default;
        bool is_member(const std::string& key) const;
        std::string as_string() const;
        Value operator[](const std::string& key) const;

        void insert(const std::string& key, const std::string& value);

    private:
        std::map<std::string, std::string> fields_;
        std::string value_;
    };

    class Reader {
    public:
        Reader()=default;
        bool parse(const std::string& json_str, Value& root);
    private:
        static std::string trim_quotes(const std::string& str);
    };
}


#endif //ASSIGNMENT3_JSONPARSER_H