#ifndef JSON_H
#define JSON_H

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <string>
#include <iostream>

#include "Dictionary.h"

class JSON {
public:
    JSON();
    ~JSON();

    void Parse(const std::string &jsonText, bool keepText = false);
    void ParseString(const std::string &jsonSting);
    std::string Stringify() const;
    const std::string &GetParsedJson() const;
    void AddDicionary(const std::string &name, Dictionary &dict);

    template <typename T>
    void SetValue(const std::string &key, const T &value) {
        rapidjson::Value jsonKey(key.c_str(), allocator);
        rapidjson::Value jsonValue;

        if constexpr (std::is_same_v<T, std::string>) {
            jsonValue.SetString(value.c_str(), allocator);
        } else if constexpr (std::is_same_v<T, const char *>) {
            jsonValue.SetString(value, allocator);
        } else if constexpr (std::is_integral_v<T>) {
            jsonValue.SetInt(value);
        } else if constexpr (std::is_floating_point_v<T>) {
            jsonValue.SetDouble(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue.SetBool(value);
        } else {
            static_assert("Unsupported type for JSON value");
        }

        if (data.HasMember(jsonKey)) {
            data[jsonKey] = jsonValue;
        } else {
            data.AddMember(jsonKey, jsonValue, allocator);
        }
    }

    template <typename T>
    void SetValue(const std::string &parentKey, const std::string &childKey, const T &value) {
        rapidjson::Value jsonValue;

        if constexpr (std::is_same_v<T, std::string>) {
            jsonValue.SetString(value.c_str(), allocator);
        } else if constexpr (std::is_same_v<T, const char *>) {
            jsonValue.SetString(value, allocator);
        } else if constexpr (std::is_integral_v<T>) {
            jsonValue.SetInt(value);
        } else if constexpr (std::is_floating_point_v<T>) {
            jsonValue.SetDouble(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue.SetBool(value);
        } else {
            static_assert("Unsupported type for JSON value");
        }

        if (!data.HasMember(parentKey.c_str())) {
            rapidjson::Value parent(rapidjson::kObjectType);
            data.AddMember(rapidjson::Value(parentKey.c_str(), allocator), parent, allocator);
        }

        auto &parentObj = data[parentKey.c_str()];

        if (!parentObj.IsObject()) {
            std::cout << "Error: Parent key is not a JSON object!" << std::endl;
            return;
        }

        rapidjson::Value jsonChildKey(childKey.c_str(), allocator);
        if (parentObj.HasMember(jsonChildKey)) {
            parentObj[jsonChildKey] = jsonValue;
        }
    }

    template <typename T>
    T GetValue(const std::string &key, const T &defaultValue) const
    {
        if (!data.HasMember(key.c_str())) {
            std::cout << "Warning: Key '" + key + "' not found in JSON data." << std::endl;
            return defaultValue;
        }

        const auto &value = data[key.c_str()];

        if constexpr (std::is_same_v<T, std::string>) {
            if (value.IsString())
                return value.GetString();
        } else if constexpr (std::is_integral_v<T>) {
            if (value.IsInt())
                return value.GetInt();
        } else if constexpr (std::is_floating_point_v<T>) {
            if (value.IsDouble())
                return value.GetDouble();
        } else if constexpr (std::is_same_v<T, bool>) {
            if (value.IsBool())
                return value.GetBool();
        }

        std::cout << "Warning: Key '" << key << "' does not contain a value of the expected type." << std::endl;
        return defaultValue;
    }

    template <typename T>
    T GetValue(const std::string &parentKey, const std::string &childKey, const T &defaultValue) const
    {
        if (!data.HasMember(parentKey.c_str())) {
            std::cout << "Warning: Parent key '" << parentKey << "' not found in JSON data." << std::endl;
            return defaultValue;
        }

        const auto &parent = data[parentKey.c_str()];

        if (!parent.IsObject()) {
            std::cout << "Warning: Parent key '" << parentKey << "' is not an object." << std::endl;
            return defaultValue;
        }

        if (!parent.HasMember(childKey.c_str())) {
            std::cout << "Warning: Child key '" << childKey << "' not found under parent key '" << parentKey << "'." << std::endl;
            return defaultValue;
        }

        const auto &value = parent[childKey.c_str()];

        if constexpr (std::is_same_v<T, std::string>) {
            if (value.IsString())
                return value.GetString();
        } else if constexpr (std::is_integral_v<T>) {
            if (value.IsInt())
                return value.GetInt();
        } else if constexpr (std::is_floating_point_v<T>) {
            if (value.IsDouble())
                return value.GetDouble();
        } else if constexpr (std::is_same_v<T, bool>) {
            if (value.IsBool())
                return value.GetBool();
        }

        std::cout << "Warning: Child key '" << childKey << "' does not contain a value of the expected type." << std::endl;
        return defaultValue;
    }

    rapidjson::Document &GetData() { return data; }
    rapidjson::Document::AllocatorType &GetAllocator() { return allocator; }

private:
    rapidjson::Document data;
    rapidjson::Document::AllocatorType &allocator;
    std::string originalJsonText;
};

#endif
