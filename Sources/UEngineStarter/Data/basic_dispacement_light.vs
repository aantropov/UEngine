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

uniform struct PointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
} light;

uniform sampler2D displacement; 

out Vertex {
	vec2  texcoord;
	vec3  normal;
	vec3  lightDir;
	vec3  viewDir;
	float distance;
} Vert;

void main(void)
{   
    vec4 delta = texture2D(displacement, texcoord);
    //float df = 0.30*delta.x + 0.59*delta.y + 0.11*delta.z; 
    vec4 new_position = vec4(position + (normal * delta.r * 10.0), 1); 

	vec4 vertex   = transform.model * vec4(position, 1.0);
	
	vec4 lightDir = light.position - vertex;
	Vert.normal   = transform.normal * normal;
	Vert.lightDir = vec3(lightDir);
	Vert.viewDir  = transform.viewPosition - vec3(vertex);
	Vert.distance = length(lightDir);
	Vert.texcoord = texcoord;
	gl_Position = transform.viewProjection * new_position;/**/
}
