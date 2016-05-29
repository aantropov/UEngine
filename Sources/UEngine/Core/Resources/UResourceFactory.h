#pragma once
#include "UResource.h"
#include "..\Utils\enum.h"
#include "..\Utils\ExportDef.h"

class UShaderProgram;

class UResourceFactory
{
protected:

    class UElement
    {
    protected:

        std::string id;
        UResource* resource;

    public:

        UElement(UResource* r, std::string path) : resource(r), id(path) {}

        UResource* GetResource() { return resource; }
        void SetResource(UResource* r) { resource = r; }
        
        std::string GetId() { return id; }
        void SetId(std::string id) { this->id = id; }

        void Free() { delete resource; }
        ~UElement() { Free(); }

    };

    vector<UElement*> resources;
    int unique_id;

public:

    UENGINE_DECLSPEC UResourceFactory() { unique_id = 0; }
    UENGINE_DECLSPEC ~UResourceFactory() { ReleaseAll(); }

    //Load resource from hard disk
    UResource UENGINE_DECLSPEC *Get(std::string path);
    bool UENGINE_DECLSPEC Add(std::string path, UResource* res);
    UResource UENGINE_DECLSPEC *Load(std::string path, UResourceType type);
    UResource UENGINE_DECLSPEC *Create(UResourceType type);
    UResource UENGINE_DECLSPEC *Create(std::string path, UResourceType type);

    void UENGINE_DECLSPEC Release(std::string path);
    void UENGINE_DECLSPEC Release(UResource *resource);
    void UENGINE_DECLSPEC ReleaseAll();
};