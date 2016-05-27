#extension GL_NV_shadow_samplers_cube : enable
#define maxLight 8

precision highp float;

#if defined(VERTEX)
	#define inout out
#elif defined(FRAGMENT)
	#define inout in
#endif

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;	
	mat3 normal;
	vec3 viewPosition;
    mat4 modelViewProjection;
} transform;

uniform struct Material
{
	samplerCube cubemap;
} material;

inout Vertex 
{
	vec4  position;
	vec2  texcoord;
	vec3  normal;
	vec3  viewDir;
	
} Vert;

#if defined(VERTEX)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 texcoord;

void main(void)
{
	vec4 vertex = transform.model * vec4(position, 1.0);
	
	Vert.texcoord = texcoord;
	Vert.position = vertex;		
	Vert.normal = transform.normal * normal;
	Vert.viewDir = normalize(vertex.xyz - transform.viewPosition.xyz);
	
    gl_Position = transform.viewProjection * (vertex);	
}

#elif defined(FRAGMENT)

#if !defined(DEFFERED)

out vec4 color;
void main(void)
{
	vec3 normal = Vert.normal;
	color = textureCube(material.cubemap, Vert.viewDir);
}
#else

out vec4 color[5];
void main(void)
{
	vec3 normal = Vert.normal;
    color[0] = textureCube(material.cubemap, Vert.viewDir);
	color[1] = vec4(0);
	color[2] = vec4(0);
	color[3] = vec4(0);
	color[4].r = 0;
}
#endif

#endif
