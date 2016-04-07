#define maxLight 8
#define maxBones 64

#if defined(VERTEX)
	#define inout out
#elif defined(FRAGMENT)
	#define inout in
#endif

#if defined(SKINNING)
uniform int skinning_transformsNum;
uniform mat4 skinning_transforms[maxBones];
#endif 

uniform struct Camera
{
   highp float zFar;
   highp float zNear;
} camera;

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;
	mat3 normal;
	vec3 viewPosition;
    mat4 modelViewProjection;
} transform;

inout Vertex {
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
} Vert;

#if defined(VERTEX)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in vec4 bone_indices;
layout(location = 5) in vec4 bone_weights;

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

	vec4 vertex = transform.model * skinnedVertex;	
#else
	vec4 vertex = transform.model * vec4(position, 1.0);
#endif

#if defined(SKINNING)
	Vert.normal = normalize(transform.normal * MVIN * normal);
#else
	Vert.normal = normalize(transform.normal * normal);
#endif

	Vert.texcoord = texcoord;
	Vert.position = transform.viewProjection * vertex;
	gl_Position = Vert.position;
}
#elif defined(FRAGMENT)

out vec2 color;
void main()
{
    float depth = Vert.position.z / Vert.position.w;
    
	depth = (Vert.position.z - camera.zNear)/(camera.zFar - camera.zNear);
    //depth = (Vert.position.z + camera.zNear);
    
    //depth = depth * 0.5 + 0.5;
    
	float moment1 = depth;
	float moment2 = depth * depth;
		
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	moment2 += 0.25*(dx*dx+dy*dy);	 
    
    color = vec2(moment1, moment2);    
}
#endif
