#pragma once
#include "unode.h"
#include "..\Renderer\URenderer.h"
#include "..\Basic\UCamera.h"
#include <vector>
#include <map>
#include "UIRenderable.h"
#include "..\Utils\ExportDef.h"
#include "..\Utils\enum.h"
#include "..\math\umath.h"
#include "UIAnimatable.h"
#include "..\GameMechanics\UGameMechanics.h"
#include "..\Utils\enum.h"

using namespace std;

class URenderer;
class ULight;
class UMaterial;
class UMesh;

class ULightParams
{
public:

    unsigned int count;
    unsigned int light_index[MAX_LIGHTS];

    vec4 position[MAX_LIGHTS];
    vec4 ambient[MAX_LIGHTS];
    vec4 diffuse[MAX_LIGHTS];
    vec4 specular[MAX_LIGHTS];
    vec3 attenuation[MAX_LIGHTS];
    vec3 spotDirection[MAX_LIGHTS];
    float spotExponent[MAX_LIGHTS];
    float spotCosCutoff[MAX_LIGHTS];
    mat4 transforms[MAX_LIGHTS];
    int types[MAX_LIGHTS];

    ULightParams() : count(0) { std::memset(light_index, 0, sizeof(unsigned int) * MAX_LIGHTS); }
};

class UScene
{
    int cursor_position[2];
    double rotate_delta[2];
    double move_delta[2];

    vector<ULight*> lights;
    map<int, list<pair<mat4,UMesh*>>> render_queue;

public:

    class USceneNode : public UNode
    {
        UGameObject* node;
        std::vector<USceneNode*> children;
        bool is_already_updated;

    public:

        UENGINE_DECLSPEC USceneNode(UGameObject* n) : node(n), is_already_updated(false) {}
        UENGINE_DECLSPEC ~USceneNode();

        void UENGINE_DECLSPEC AddChild(USceneNode* n);
        void UENGINE_DECLSPEC AddToRenderQueue(map<int, list<pair<mat4, UMesh*>>>& render_queue);
        void UENGINE_DECLSPEC Update(double delta);
        void UENGINE_DECLSPEC DeleteTopology();
        void UENGINE_DECLSPEC Free();
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
    ULightParams light_params;

    void UENGINE_DECLSPEC KeysProccessing();

    void UENGINE_DECLSPEC Update(double deltaTime);
    void UENGINE_DECLSPEC UpdateLightParams();

    void UENGINE_DECLSPEC PrepareRenderQueue();
    void UENGINE_DECLSPEC RenderQueue(URENDER_PASS type, UCamera camera);

    //void UENGINE_DECLSPEC Render(URENDER_PASS type, UCamera camera);
    
    UENGINE_DECLSPEC UScene(void) : root(NULL)
    {
        rotate_delta[0] = rotate_delta[1] = 0.0;
        move_delta[0] = move_delta[1] = 0.0;
    }

    void UENGINE_DECLSPEC Free()
    {
        root->DeleteTopology();
        delete root;
    }

    UENGINE_DECLSPEC ~UScene(void);
};