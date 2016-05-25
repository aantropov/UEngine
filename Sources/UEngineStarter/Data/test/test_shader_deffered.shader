#extension GL_NV_shadow_samplers_cube : enable
#define maxLight 8

precision highp float;

#if defined(VERTEX)
	#define inout out
#elif defined(FRAGMENT)
	#define inout in
#endif

uniform struct Camera
{
   highp float zFar;
   highp float zNear;
} camera;

uniform struct Transform
{
	mat4 model;
	mat4 view;
	mat4 viewProjection;
	mat3 normal;
	vec3 viewPosition;
    mat4 modelViewProjection;
} transform;

uniform struct Material
{
	sampler2D texture;
	sampler2D specular_tex;
	
#if defined(NORMAL_MAPPING)
	sampler2D normal;
#endif

	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
	
#if defined(REFLECTION_CUBEMAP)
	samplerCube cubemap;
#endif

} material;

inout Vertex 
{
	highp vec4  position;
	vec2  texcoord;
	vec3  normal;
	vec3 t;
	vec3 b;
	vec3 viewDir;
} Vert;

#if defined(SKINNING)
uniform int skinning_transformsNum;
uniform mat4 skinning_transforms[maxBones];
#endif 

#if defined(VERTEX)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;

void main(void)
{
#if defined(SKINNING)
	vec4 skinnedVertex = vec4(0, 0, 0, 0);
	vec4 vertex = vec4(position, 1.0);
	
	mat4 MVI = bone_weights[0]*skinning_transforms[int(bone_indices[0])];
	MVI += bone_weights[1]*skinning_transforms[int(bone_indices[1])];
	MVI += bone_weights[2]*skinning_transforms[int(bone_indices[2])];
	MVI += bone_weights[3]*skinning_transforms[int(bone_indices[3])];
	skinnedVertex = MVI * vertex;	
	mat3 MVIN = mat3(transpose(inverse(MVI)));

	vertex = transform.model * skinnedVertex;	
#else
	vec4 vertex = transform.model * vec4(position, 1.0);
#endif	
	
	Vert.position = transform.view * vertex;	
	Vert.texcoord = texcoord;
		
	vec3 n = transform.normal * normal;
	Vert.b = transform.normal * binormal;
	Vert.t = (cross(n, Vert.b));

#if defined(SKINNING)
	Vert.normal = transform.normal * MVIN * normal;
#else
	Vert.normal = transform.normal * normal;
#endif

	Vert.viewDir = normalize(vec3(transform.viewPosition - vec3(vertex)));
	gl_Position = transform.viewProjection * (vertex);	
}

#elif defined(FRAGMENT)

out vec4 color[5];

void main(void)
{
	vec3 normal = Vert.normal;
	mat3 tbn = transpose(mat3((Vert.t), (Vert.b), (Vert.normal)));
	
#ifdef NORMAL_MAPPING
	normal = texture(material.normal, Vert.texcoord).xyz * 2.0 - 1.0;  
	normal *= tbn;	
#endif
	normal = normalize(normal);

	vec4 specular = texture(material.specular_tex, Vert.texcoord);
	
    color[0] = material.emission;
    
#if defined(REFLECTION_CUBEMAP)
    vec3 viewDir = normalize(Vert.viewDir);
    vec3 reflectDir = normalize(reflect(viewDir, normal));
    vec4 reflectColor = vec4(textureCube(material.cubemap, reflectDir)) * pow(1 - dot(Vert.normal, viewDir), 3);
    color[0] += reflectColor;
#endif

	color[1] = vec4(normal * 0.5 + vec3(0.5), 1);
	color[2] = material.diffuse * texture(material.texture, Vert.texcoord);
	color[3] = vec4(material.specular.xyz * specular.xyz, material.shininess / 255.0f);// * material.specular.w * specular.w;
	color[4].r = -(Vert.position.z - camera.zNear)/(camera.zFar - camera.zNear);
}
#endif
