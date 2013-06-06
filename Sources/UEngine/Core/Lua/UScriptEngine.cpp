#include "UScriptEngine.h"
#include "UScript.h"
#include <string>
#include "..//Basic/URegistry.hpp"
#include "..//GameMechanics/UGameMechanics.h"

UScriptEngine:: UScriptEngine() {}

UScriptEngine:: ~UScriptEngine()
{
    ULogger::GetInstance()->Message("UScriptEngine singleton object is deleted...");
}

UScriptEngine* UScriptEngine:: GetInstance()
{
    if(instance == NULL){
        ULogger::GetInstance()->Message("UScriptEngine singleton object is created...");
        instance = new UScriptEngine();
    }
    return instance;
}

UGameObject* UScriptEngine::GetGameObjectByComponent(UComponent* component){ return component->gameObject; }
//UComponent* UScriptEngine::GetComponent(UGameObject* gameObject, std::string name){ }
UGameObject* UScriptEngine::GetParent(UGameObject* gameObject){ return gameObject->parentObject; }
std::vector<UGameObject*> UScriptEngine::GetChildren(UGameObject* gameObject){ return gameObject->children; }
bool UScriptEngine::SetTransform(UGameObject* gameObject, transform local){ gameObject->local = local; return true; }
transform UScriptEngine::GetTransform(UGameObject* gameObject){ return gameObject->local; }
transform UScriptEngine::GetWorldTransform(UGameObject* gameObject){ return gameObject->world; }

static bool SetName(UGameObject* gameObject, std::string name){ gameObject->name = name; return true; }
static std::string GetName(UGameObject* gameObject){ return gameObject->name; }

void UScriptEngine::RegisterFunctions(UScript* script)
{
    URegistry<lua_State*, UScript*>::Registry[script->GetLuaVM()] = script;

    WrapFunc1<bool, std::string, UScriptEngine::MessageBoxW>().registerFunc(script->GetLuaVM(), "MessageBox");
    WrapFuncObj0<UGameObject*, UScript, &UScript::GetGameObject>().registerFunc(script->GetLuaVM(), "GetGameObject");
    WrapFunc1<UGameObject*, UComponent*, UScriptEngine::GetGameObjectByComponent>().registerFunc(script->GetLuaVM(), "GetGameObjectByComponent");
    WrapFunc1<UGameObject*, UGameObject*, UScriptEngine::GetParent>().registerFunc(script->GetLuaVM(), "GetParent");
    WrapFunc1<std::vector<UGameObject*>, UGameObject*, UScriptEngine::GetChildren>().registerFunc(script->GetLuaVM(), "GetChildren");
    WrapFunc2<bool, UGameObject*, transform, UScriptEngine::SetTransform>().registerFunc(script->GetLuaVM(), "SetTransform");
    WrapFunc1<transform, UGameObject*, UScriptEngine::GetTransform>().registerFunc(script->GetLuaVM(), "GetTransform");
    WrapFunc1<transform, UGameObject*, UScriptEngine::GetWorldTransform>().registerFunc(script->GetLuaVM(), "GetWorldTransform");
    
    //WrapFunc2<bool, UGameObject*, string, UScriptEngine::SetName>().registerFunc(script->GetLuaVM(), "SetName");
    //WrapFunc1<string, UGameObject*, UScriptEngine::GetName>().registerFunc(script->GetLuaVM(), "GetName");
    
}
