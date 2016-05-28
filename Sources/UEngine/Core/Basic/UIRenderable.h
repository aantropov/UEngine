#pragma once
#include "UMatrix.h"
#include "..\Utils\enum.h"
#include <vector>
#include <map>

using namespace std;

class UMaterial;
class UMesh;

class UIRenderable
{
protected:
public:
    
    UMatrix render_transform;

    virtual void Render(URENDER_PASS type, int light_index = 0) = 0;
    virtual void Render(UMaterial* m) = 0;
    virtual void AddToRenderQueue(map<int, vector<pair<mat4,UMesh*>>>& render_queue) = 0;

    UIRenderable(void);
    virtual ~UIRenderable(void);
};