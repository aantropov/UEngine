#pragma once
#include "unode.h"
#include "..\Renderer\URenderer.h"
#include "..\Basic\UCamera.h"
#include <vector>
#include "UIRenderable.h"
#include "..\Utils\ExportDef.h"
#include "..\Utils\enum.h"
#include "..\math\umath.h"
#include "UIAnimatable.h"
#include "..\GameMechanics\UGameMechanics.h"

using namespace std;

class URenderer;
class ULight;
class UMaterial;

class ULightParams
{
public:

    unsigned int count;
    unsigned int lightIndex[MAX_LIGHTS];

    vec4 position[MAX_LIGHTS];
    vec4 ambient[MAX_LIGHTS];
    vec4 diffuse[MAX_LIGHTS];
    vec4 specular[MAX_LIGHTS];
    vec3 attenuation[MAX_LIGHTS];
    vec3 spotDirection[MAX_LIGHTS];
    float spotExponent[MAX_LIGHTS];
    float spotCosCutoff[MAX_LIGHTS];
    mat4 transforms[MAX_LIGHTS];

    ULightParams() : count(0) { std::memset(lightIndex, 0, sizeof(unsigned int) * MAX_LIGHTS); }
};

class UScene
{

    int cursorPos[2];
    double rotateDelta[2];
    double moveDelta[2];

    vector<ULight*> lights;

public:

    class USceneNode : public UNode
    {
        UGameObject* node;
        std::vector<USceneNode*> children;
        bool isAlreadyUpdated;

    public:

        UENGINE_DECLSPEC USceneNode(UGameObject* n) : node(n), isAlreadyUpdated(false) {}

        void UENGINE_DECLSPEC AddChild(USceneNode* n)
        {
            node->children.push_back(n->node);
            children.push_back(n);
        }

        void UENGINE_DECLSPEC Render(URENDER_TYPE type)
        {
            node->Render(type);
            for (unsigned int i = 0; i < children.size(); i++) {
                if (children[i] != NULL)
                    children[i]->Render(type);
            }
        }

        void UENGINE_DECLSPEC Render(UMaterial *m)
        {
            node->Render(m);
            for (unsigned int i = 0; i < children.size(); i++) {
                if (children[i] != NULL)
                    children[i]->Render(m);
            }
        }

        void UENGINE_DECLSPEC Update(double delta)
        {
            if (node != NULL && !isAlreadyUpdated)
            {
                node->Update(delta);
                isAlreadyUpdated = true;
            }

            for (unsigned int i = 0; i < children.size(); i++)
            {
                children[i]->node->parentObject = node;
                children[i]->node->world = node->local * node->world;
                children[i]->Update(delta);
            }

            isAlreadyUpdated = false;
        }

        void UENGINE_DECLSPEC DeleteTopology()
        {
            for (unsigned int i = 0; i < children.size(); i++) {
                USceneNode *temp = NULL;
                temp = dynamic_cast<USceneNode*>(children[i]);

                if (temp != NULL) {
                    temp->DeleteTopology();
                    delete temp;
                }
            }
        }

        void UENGINE_DECLSPEC Free()
        {
            children.clear();
        }

        UENGINE_DECLSPEC ~USceneNode()
        {
            Free();
        }
    };

    void UENGINE_DECLSPEC AddLight(ULight* n)
    {
        lights.push_back(n);
    }

    vector<ULight*> UENGINE_DECLSPEC GetLights()
    {
        return lights;
    }

    USceneNode *root;
    ULightParams lightParams;

    void UENGINE_DECLSPEC KeysProccessing();

    void UENGINE_DECLSPEC Update(double deltaTime);
    void UENGINE_DECLSPEC UpdateLightParams();

    void UENGINE_DECLSPEC Render(URENDER_TYPE type, UCamera camera);
    void UENGINE_DECLSPEC Render(UMaterial *m, UCamera camera);

    UENGINE_DECLSPEC UScene(void) : root(NULL)
    {
        rotateDelta[0] = rotateDelta[1] = 0.0;
        moveDelta[0] = moveDelta[1] = 0.0;
    }

    void UENGINE_DECLSPEC Free()
    {
        root->DeleteTopology();
        delete root;
    }

    UENGINE_DECLSPEC ~UScene(void);
};