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
        ULogger::GetInstance()->Message("Error to load resource: " + path, ULOG_MSG_ERROR, ULOG_OUT_MSG);
        return false;
    }
    return true;
}