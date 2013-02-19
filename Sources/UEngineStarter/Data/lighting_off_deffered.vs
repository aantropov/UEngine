#version 330 core
#define maxLight 8

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 texcoord;

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;	
	mat3 normal;
} transform;

out Vertex {
	vec4 position;
	vec2 texcoord;
	vec3 normal;
} Vert;

void main(void)
{
	vec4 vertex   = transform.model * vec4(position, 1.0);
	Vert.normal   = normalize(transform.normal * normal);
	
	Vert.texcoord = texcoord;
	Vert.position = vertex;

	gl_Position = transform.viewProjection * (vertex);	
}
