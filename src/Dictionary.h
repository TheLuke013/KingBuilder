#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <map>
#include <string>
#include <variant>

class Dictionary {
public:
    using ValueType = std::variant<int, double, std::string, bool>;

    Dictionary();
    ~Dictionary();

    void Add(const std::string& key, const ValueType& value);
    ValueType Get(const std::string& key) const;
    bool Contains(const std::string& key) const;
    void Remove(const std::string& key);
    const std::map<std::string, ValueType>& GetMap() const { return dict; }

private:
    std::map<std::string, ValueType> dict;

};

#endif