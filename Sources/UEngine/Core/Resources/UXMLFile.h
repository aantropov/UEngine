#pragma once

#include <vector>
#include <string>
#include <stdio.h>
using namespace std;

#include "..\Utils\enum.h"
#include "UResource.h"
#include "..\Utils\ULogger.h"

class UXMLFile :  public UResource
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

    std::string GetPath();
    bool Load(std::string path);
    void Free(){}

    /* Get value by "path": /a/b/c
    for xml:
    <a><a1></a1><b><c>Test<c></b></a>
    result:Test
    */
    std::string GetElement(std::string path);
	int GetElementi(std::string param_path);
	float GetElementf(std::string param_path);

    vector<string> GetElements(std::string key);
    bool isExistElement(string key);
};

