#pragma once

template<typename T>
class USingleton 
{
protected:

    static T* instance;
    USingleton(){}

public:

    static T* GetInstance();
    static void Free()
    {
        delete instance;
    }
};

template<typename T>
T* USingleton<T>:: instance = nullptr;
