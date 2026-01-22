#include "Dictionary.h"

#include <iostream>

Dictionary::Dictionary() {

}

Dictionary::~Dictionary() {

}

void Dictionary::Add(const std::string& key, const ValueType& value) {
    dict[key] = value;
}

Dictionary::ValueType Dictionary::Get(const std::string& key) const {
    if (dict.find(key) != dict.end()) {
        return dict.at(key);
    }
    else {
        std::cout << "Key Not Found: " << key << std::endl;
        return {};
    }
}

bool Dictionary::Contains(const std::string& key) const {
    return dict.find(key) != dict.end();
}

void Dictionary::Remove(const std::string& key) {
    dict.erase(key);
}