#version 330 core
#define maxLight 8

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;

uniform int lightsNum;

uniform vec4 light_position[maxLight];
uniform mat4 light_transform[maxLight];

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;	
	mat3 normal;
	vec3 viewPosition;
        mat4 modelViewProjection;
} transform;

out Vertex 
{
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
	vec4 vertex = transform.model * vec4(position, 1.0);
		
	Vert.texcoord = texcoord;
	Vert.position = vertex;		
	
	Vert.transformNormal = transform.normal;

	vec3 n = Vert.transformNormal * normal;
	Vert.b = Vert.transformNormal * binormal;
	Vert.t = (cross(n, Vert.b));

	Vert.normal = normalize(n);
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
	
	/*for(int i = 0; i < min(maxLight, lightsNum); i++)
	{
		ProcessLight(i, vertex, Vert.t, Vert.b, n);
	}
	*/
	
	gl_Position = transform.viewProjection * (vertex);	
}
