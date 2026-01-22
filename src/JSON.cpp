#include "JSON.h"

JSON::JSON()
    : allocator(data.GetAllocator()) {
    data.SetObject();
}

JSON::~JSON() { }

void JSON::Parse(const std::string &jsonText, bool keepText) {
    if (keepText)
        originalJsonText = jsonText;

    if (data.Parse(jsonText.c_str()).HasParseError()) {
        std::cout << "Fatal Error: Failed to parse JSON" << std::endl;
    }
}

std::string JSON::Stringify() const {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    data.Accept(writer);
    return buffer.GetString();
}

const std::string &JSON::GetParsedJson() const {
    return originalJsonText;
}

void JSON::AddDicionary(const std::string &name, Dictionary &dict) {
    rapidjson::Value jsonDict(rapidjson::kObjectType);

    for (const auto &[key, value] : dict.GetMap()) {
        rapidjson::Value jsonKey(key.c_str(), allocator);
        rapidjson::Value jsonValue;

        if (std::holds_alternative<int>(value)) {
            jsonValue.SetInt(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            jsonValue.SetDouble(std::get<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            jsonValue.SetString(std::get<std::string>(value).c_str(), allocator);
        } else if (std::holds_alternative<bool>(value)) {
            jsonValue.SetBool(std::get<bool>(value));
        }

        jsonDict.AddMember(jsonKey, jsonValue, allocator);
    }

    rapidjson::Value jsonName(name.c_str(), allocator);
    data.AddMember(jsonName, jsonDict, allocator);
}
