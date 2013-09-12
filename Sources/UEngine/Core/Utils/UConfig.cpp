#include "UConfig.h"

UConfig::UConfig()
{
    uCfg.Load(UCONFIG_FILE_PATH);
}


UConfig::~UConfig()
{
    ULogger::GetInstance()->Message("UConfig singleton object is deleted...");
}

UConfig* UConfig::GetInstance()
{
    if(instance == NULL)
    {
        ULogger::GetInstance()->Message("UConfig singleton object is created...");
        instance = new UConfig();        
    }
    return instance;
}

std::string UConfig::GetParam(std::string param_path) 
{
    return uCfg.GetElement(param_path);
}