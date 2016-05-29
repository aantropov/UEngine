#pragma once
#include "UMatrix.h"
#include "..\Utils\enum.h"
#include <vector>
#include <map>
#include <list>

using namespace std;

class UMaterial;
class UMesh;
class URenderQueue;

class UIRenderable
{
protected:
public:
    
    virtual void Render(URENDER_PASS type, int light_index = 0) = 0;   
    virtual void AddToRenderQueue(URenderQueue& render_queue) = 0;

    UIRenderable(void);
    virtual ~UIRenderable(void);
};