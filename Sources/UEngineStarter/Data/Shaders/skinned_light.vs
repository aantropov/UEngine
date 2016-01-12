#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in vec4 bone_indices;
layout(location = 5) in vec4 bone_weights;

#define maxLight 8
#define maxBones 64

uniform int lightsNum;

uniform int skinning_transformsNum;
uniform mat4 skinning_transforms[maxBones];

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;	
	mat3 normal;
	vec3 viewPosition;
        mat4 modelViewProjection;
} transform;

uniform vec4 light_position[maxLight];
uniform mat4 light_transform[maxLight];

out Vertex {
	vec4  position;
	vec2  texcoord;
	vec3  normal;
	vec3  lightDir[maxLight];
	vec3  lightDirTBN[maxLight];
	vec3  viewDirTBN;
	vec3  viewDir;
	vec4 smcoord[maxLight];	
	vec3 t;
	vec3 b;
	mat3 transformNormal;
} Vert;

void ProcessLight(int i, vec4 vertex, vec3 t, vec3 b, vec3 n)
{
	Vert.smcoord[i]  = light_transform[i] * vertex;
	vec3 lightDir = vec3(light_position[i] - vertex);
	
	Vert.lightDir[i] = lightDir;

	Vert.lightDirTBN[i].x = dot(lightDir, t);
	Vert.lightDirTBN[i].y = dot(lightDir, b);
	Vert.lightDirTBN[i].z = dot(lightDir, n);
}

void main(void)
{
	vec4 skinnedVertex=vec4(0,0,0,0);
	vec4 vert = vec4(position, 1.0);
	
	mat4 MVI = bone_weights[0] * skinning_transforms[int(bone_indices[0])];
	MVI += bone_weights[1] * skinning_transforms[int(bone_indices[1])];
	MVI += bone_weights[2] * skinning_transforms[int(bone_indices[2])];
	MVI += bone_weights[3] * skinning_transforms[int(bone_indices[3])];
	skinnedVertex = MVI * vert;	
	mat3 MVIN = mat3(transpose(inverse(MVI)));

	vec4 vertex   =  transform.model * skinnedVertex;	   
	Vert.position = vertex;
	
	Vert.transformNormal =  transform.normal * MVIN;

	vec3 n = Vert.transformNormal * normal;
	Vert.b = Vert.transformNormal * binormal;
	Vert.t = (cross(n, Vert.b));
	
	Vert.normal = normalize(n);
	
	Vert.texcoord = texcoord;
	Vert.viewDir = normalize(vec3(transform.viewPosition - vec3(vertex)));
	
	Vert.viewDirTBN.x = dot(Vert.viewDir, Vert.t);
	Vert.viewDirTBN.y = dot(Vert.viewDir, Vert.b);
	Vert.viewDirTBN.z = dot(Vert.viewDir, n);

	ProcessLight(0, vertex, Vert.t, Vert.b, n);
	ProcessLight(1, vertex, Vert.t, Vert.b, n);
	ProcessLight(2, vertex, Vert.t, Vert.b, n);
	ProcessLight(3, vertex, Vert.t, Vert.b, n);
	ProcessLight(4, vertex, Vert.t, Vert.b, n);
	ProcessLight(5, vertex, Vert.t, Vert.b, n);
	ProcessLight(6, vertex, Vert.t, Vert.b, n);
	ProcessLight(7, vertex, Vert.t, Vert.b, n);
	
	/*for(int i = 0; i < lightsNum; i++)
		ProcessLight(i, vertex, t, b, n);*/
	
	gl_Position = transform.viewProjection * (vertex);
}
