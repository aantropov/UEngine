#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;

#define maxLight 8
uniform int lightsNum;

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;	
	mat3 normal;
	vec3 viewPosition;
} transform;

uniform vec4 light_position[maxLight];
uniform mat4 light_transform[maxLight];

out Vertex {	
	vec2  texcoord;
	vec3  normal;
	vec3  lightDir[maxLight];
	vec3  lightDirTBN[maxLight];
	vec3  viewDir;
	vec4 smcoord[maxLight];	
} Vert;

void ProcessLight(int i, vec4 vertex, vec3 t, vec3 b, vec3 n)
{
	Vert.smcoord[i]  = light_transform[i] * vertex;
	vec3 lightDir = vec3(light_position[i] - vertex);
	
	Vert.lightDirTBN[i].x = dot(lightDir, t);
	Vert.lightDirTBN[i].y = dot(lightDir, b);
	Vert.lightDirTBN[i].z = dot(lightDir, n);

	Vert.lightDir[i] = lightDir;
}

void main(void)
{
	vec4 vertex   = transform.model * vec4(position, 1.0);
	Vert.normal   = normalize(transform.normal * normal);
	
	vec3 n = (transform.normal * normal);
	vec3 b = (transform.normal * binormal);
	vec3 t = (cross(n, b));

	Vert.viewDir.x = dot(vec3(-vertex), t);
	Vert.viewDir.y = dot(vec3(-vertex), b);
	Vert.viewDir.z = dot(vec3(-vertex), n);
	
	vec3 lightDir;
	for(int i = 0; i < int(lightsNum) ; i++)
    {
		ProcessLight(i, vertex, t, b, n);
	}

	Vert.texcoord = texcoord;
	gl_Position = transform.viewProjection * (vertex);	

}
