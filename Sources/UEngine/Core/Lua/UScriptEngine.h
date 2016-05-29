#pragma once

#include "..\Basic\USingleton.hpp"
#include "..\Utils\ULogger.h"
#include "..\Utils\ExportDef.h"

#include "LuaTemplates.hpp"

class UScript;
class UGameObject;
class UComponent;
struct transform;

class UScriptEngine : public USingleton<UScriptEngine>
{
public:
    
    static UScriptEngine* GetInstance();
    
    static bool MsgBox(std::string text)
    {
        ULogger::GetInstance()->Message(text, ULogType::Info, ULogTarget::MsgBox);
        return true;
    }
    
    //GameObject Methods
    static UGameObject* GetGameObjectByComponent(UComponent* component);
    static UComponent* GetComponent(UGameObject* gameObject, std::string name);
    static UGameObject* GetParent(UGameObject* gameObject);
    static std::vector<UGameObject*> GetChildren(UGameObject* gameObject);
    
    static bool SetTransform(UGameObject* gameObject, ::transform local);
    static ::transform GetTransform(UGameObject* gameObject);
    static ::transform GetWorldTransform(UGameObject* gameObject);

    static bool SetName(UGameObject* gameObject, std::string name);
    static std::string GetName(UGameObject* gameObject);

    void UENGINE_DECLSPEC RegisterFunctions(UScript* script);

    void UENGINE_DECLSPEC Release(){}

    UScriptEngine();
    ~UScriptEngine();
};
