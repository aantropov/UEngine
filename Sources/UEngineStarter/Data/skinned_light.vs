#version 330 core

#define maxLight 8
#define maxBones 64

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in vec4 bone_indices;
layout(location = 5) in vec4 bone_weights;


uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;
	mat3 normal;
	vec3 viewPosition;
} transform;

uniform struct PointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
} light;


out Vertex {
	vec2  texcoord;
	vec3  normal;
	vec3  lightDir;
	vec3  viewDir;
	float distance;
} Vert;

uniform int skinning_transformsNum;
uniform mat4 skinning_transforms[maxBones];

void main(void)
{
	vec4 skinnedVertex=vec4(0,0,0,0);
	vec4 vert = vec4(position, 1.0);
	
	mat4 MVI = bone_weights[0]*skinning_transforms[int(bone_indices[0])];
	MVI += bone_weights[1]*skinning_transforms[int(bone_indices[1])];
	MVI += bone_weights[2]*skinning_transforms[int(bone_indices[2])];
	MVI += bone_weights[3]*skinning_transforms[int(bone_indices[3])];
	skinnedVertex = MVI*vert;	
	mat3 MVIN = mat3(transpose(inverse(MVI)));

	vec4 vertex   =  transform.model*skinnedVertex;//vec4(position, 1.0);
	Vert.normal   = normalize(transform.normal * MVIN * normal);

	vec4 lightDir = light.position - vertex;
	
	Vert.lightDir = vec3(lightDir);
	Vert.viewDir  = transform.viewPosition - vec3(vertex);
	Vert.distance = length(lightDir);
	Vert.texcoord = texcoord;
	gl_Position = transform.viewProjection * vertex;
}
