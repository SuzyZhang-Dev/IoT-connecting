//
// Created by 张悦 on 20.4.2026.
//

#include "JsonParser.h"

namespace JsonParser {
    bool Value::is_member(const std::string& key) const {
        return fields_.find(key) != fields_.end();
    }

    std::string Value::as_string() const {
        return value_;
    }

    Value Value::operator[](const std::string& key) const {
        Value v;
        auto it = fields_.find(key);
        if (it != fields_.end()) {
            v.value_ = it->second;
        }
        return v;
    }

    void Value::insert(const std::string& key, const std::string& value) {
        fields_[key] = value;
    }

    std::string Reader::trim_quotes(const std::string &str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        std::string trimmed = str.substr(first, last - first + 1);
        if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
            return trimmed.substr(1, trimmed.length() - 2);
        }
        return trimmed;
    }

    bool Reader::parse(const std::string& json_str, Value& root) {
        size_t start = json_str.find('{');
        size_t end = json_str.find('}');
        // not a Json if no { }
        if (start == std::string::npos || end == std::string::npos) return false;
        // next char after { should be ", before } should be "
        std::string content = json_str.substr(start + 1, end - start - 1);
        size_t pos = 0;

        while (pos < content.length()) {
            // {"name": "D1", "state": "ON"}
            size_t colon = content.find(':', pos);
            if (colon == std::string::npos) break;

            std::string key_raw = content.substr(pos, colon - pos);
            size_t comma = content.find(',', colon);
            std::string val_raw;

            if (comma != std::string::npos) {
                val_raw = content.substr(colon + 1, comma - colon - 1);
                pos = comma + 1;
            } else {
                val_raw = content.substr(colon + 1);
                pos = content.length();
            }

            root.insert(trim_quotes(key_raw), trim_quotes(val_raw));
        }
        return true;
    }
}