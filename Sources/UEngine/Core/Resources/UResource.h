//Master class for resources, it should be used in manufactures
#pragma once
#include <vector>
#include <string>
#include <stdio.h>
using namespace std;

class UResourceFactory;
class UXMLFile;

class UResource
{
public:

    std::string resourceId;
    UResourceFactory* rf;

    virtual bool LoadFromFile(std::string path);
    virtual bool Load(UXMLFile& xml, std::string xmlPath) = 0;
    virtual void Free() {}

    UResource() : resourceId(""), rf(nullptr) {}
    virtual ~UResource();
};