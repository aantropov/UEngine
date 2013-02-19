#pragma once
#include "..\math\UMath.h"
#include "..\Utils\ExportDef.h"

// Vertex
class UVertex
{
	// Position
	vec3 pos;	
	// Normal
	vec3 normal;	
	//Texcoords
	vec2 texcoord;
	//Binormal
	vec3 binormal;
	
	//Bones
	vec4 bone_index;
	vec4 bone_weight;

public:

	static const int offsetPos = sizeof(vec3) + sizeof(vec3) + sizeof(vec2) + sizeof(vec4) + sizeof(vec4);

	// Position setters and getters
	void UENGINE_DECLSPEC SetPosition(vec3 p){  pos = p; }
	vec3 UENGINE_DECLSPEC GetPosition(){ return pos; }

	// Normal setters and getters
	void UENGINE_DECLSPEC SetNormal(vec3 p){  pos = p; }
	vec3 UENGINE_DECLSPEC GetNormal(){ return pos; }
	
	// Texcoord setters and getters
	void UENGINE_DECLSPEC SetTexcoord(vec2 t){  texcoord = t; }
	vec2 UENGINE_DECLSPEC GetTexcoord(){ return texcoord; }

	// Binormal setters and getters
	void UENGINE_DECLSPEC SetBinormal(vec3 bn){  binormal = bn; }
	vec3 UENGINE_DECLSPEC GetBinormal(){ return binormal; }

	// Bones
	void UENGINE_DECLSPEC SetBoneIndex(vec4 bn){  bone_index = bn; }
	vec3 UENGINE_DECLSPEC GetBoneIndex(){ return bone_index; }

	void UENGINE_DECLSPEC SetBoneWeight(vec4 bn){  bone_weight = bn; }
	vec3 UENGINE_DECLSPEC GetBoneWeight(){ return bone_weight; }

	UENGINE_DECLSPEC UVertex(vec3 p): pos(p), normal(vec3_zero), texcoord(vec2_zero), binormal(vec3_zero) {}	
	UENGINE_DECLSPEC UVertex(vec3 p, vec3 n): pos(p), normal(n), texcoord(vec2_zero), binormal(vec3_zero) {}	
	UENGINE_DECLSPEC UVertex(vec3 p, vec3 n, vec2 tc): pos(p), normal(n), texcoord(tc), binormal(vec3_zero) {}	
	UENGINE_DECLSPEC UVertex(vec3 p, vec3 n, vec2 tc, vec3 bin): pos(p), normal(n), texcoord(tc), binormal(bin) {}
	UENGINE_DECLSPEC UVertex(vec3 p, vec3 n, vec2 tc, vec3 bin, vec4 b_id, vec4 b_w): pos(p), normal(n), texcoord(tc), binormal(bin), bone_index(b_id), bone_weight(b_w) {}
	UENGINE_DECLSPEC UVertex(void);
	UENGINE_DECLSPEC ~UVertex(void);
};

