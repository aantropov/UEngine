#pragma once
#include "uengine.h"
#include "urenderer.h"
#include "umesh.h"

// Just Plane
class UPlane :
	public UMesh
{

public:

	virtual void PreRender(){
		UMesh::PreRender();
	}
	virtual void Render(){
		UMesh::Render();
	}
	
	UPlane(UResourceFactory *rf, vec3 v1, vec3 v2, vec3 v3, vec3 v4){
		// basic shaders
		sp.CreateShaderProgram(rf, "basic.vs","basic.ps");

		ib.Create(2);
		vb.Create(4);

		unsigned int * indices = reinterpret_cast<unsigned int*>(ib.GetPointer());

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
	
		indices[3] = 3;
		indices[4] = 0;
		indices[5] = 2;

		UVertex *vertices = reinterpret_cast<UVertex*>(vb.GetPointer());
	
		vertices[0] = UVertex(v1, vec3(0,1,0));
		vertices[1] = UVertex(v2, vec3(0,1,0));
		vertices[2] = UVertex(v3, vec3(0,1,0));
		vertices[3] = UVertex(v4, vec3(0,1,0));
		
		UMesh::Initialize();	

	}
	~UPlane(void);
};

