#include "UResource.h"
#include "UXMLFile.h"

UResource::~UResource(void)
{
    Free();
}

bool UResource::LoadFromFile(std::string path)
{
    UXMLFile xml;
    try {
        xml.LoadFromFile(path);
        Load(xml, "/xml/");
    }
    catch (exception e)
    {
        ULogger::GetInstance()->Message("Error to load resource: " + path, ULogType::Error, ULogTarget::MsgBox);
        return false;
    }
    return true;
}