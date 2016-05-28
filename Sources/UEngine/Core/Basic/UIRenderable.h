#pragma once
#include "UMatrix.h"
#include "..\Utils\enum.h"
#include <vector>
#include <map>
#include <list>

using namespace std;

class UMaterial;
class UMesh;

class UIRenderable
{
protected:
public:
    
    virtual void Render(URENDER_PASS type, int light_index = 0) = 0;   
    virtual void AddToRenderQueue(map<int, list<pair<mat4,UMesh*>>>& render_queue) = 0;

    UIRenderable(void);
    virtual ~UIRenderable(void);
};