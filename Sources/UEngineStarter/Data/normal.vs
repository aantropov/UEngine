#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;	
	mat3 normal;
	vec3 viewPosition;
} transform;

out Vertex {	
	vec2  texcoord;
	vec3  normal;
	vec3 t;
	vec3 b;
} Vert;

void main(void)
{
	vec4 vertex   = transform.model * vec4(position, 1.0);
	Vert.normal   = normalize(transform.normal * normal);
	
	vec3 n = (transform.normal * normal);
	Vert.b = (transform.normal * binormal);
	Vert.t = (cross(n, Vert.b));
	
	Vert.texcoord = texcoord;
	gl_Position = transform.viewProjection * (vertex);	
}
