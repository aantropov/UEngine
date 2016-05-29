#pragma once

#include <vector>
#include <string>
#include <stdio.h>
using namespace std;

#include "..\Utils\enum.h"
#include "UResource.h"
#include "..\Utils\ULogger.h"

class UXMLFile : public UResource
{
private:

    FILE* fXml;
    std::string sPath;

    struct UXMLNode
    {
        UXMLNode(std::string name1, std::string value1) : name(name1), value(value1) {}
        std::string name;
        std::string value;
    };

    bool OpenFile(std::string path);
    void CloseFile();

    vector<UXMLNode> elements;

public:

    UXMLFile();
    ~UXMLFile();

    std::string GetPath() const;
    virtual bool LoadFromFile(std::string path) override;
    virtual bool Load(UXMLFile &xml, std::string path) { return false; }

    void Free() {}

    /* Get value by "path": /a/b/c
    for xml:
    <a><a1></a1><b><c>Test<c></b></a>
    result:Test
    */
    std::string GetElement(std::string path) const;
    int GetElementi(std::string param_path) const;
    bool GetElementb(std::string param_path) const;
    float GetElementf(std::string param_path) const;

    template<class EnumType, class HelperType>
    EnumType GetElementEnum(std::string path) const
    {
        return HelperType::FromString(GetElement(path).c_str());
    }

    template<class EnumType, class HelperType>
    bool TryGetElementEnum(std::string path, EnumType &var) const
    {
        if (!isExistElement(path))
            return false;
        var = GetElementEnum<EnumType, HelperType>(path);
        return true;
    }

    bool TryGetElement(std::string path, std::string &var) const;
    bool TryGetElementi(std::string path, int &var) const;
    bool TryGetElementf(std::string path, float &var) const;
    bool TryGetElementb(std::string path, bool &var) const;

    vector<string> GetElements(std::string key) const;
    bool isExistElement(string key) const;
    bool isExistElementStrict(string key) const;
};

