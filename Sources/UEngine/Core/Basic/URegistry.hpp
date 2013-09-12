#pragma once
#include <map>

template<typename Key, typename Value>
class URegistry
{
public:

    static std::map<Key, Value> Registry;

    URegistry(){}
    ~URegistry(){}    
};

template<typename Key, typename Value> std::map<Key, Value> URegistry<Key,Value>:: Registry;