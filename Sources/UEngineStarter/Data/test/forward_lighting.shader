#extension GL_NV_shadow_samplers_cube : enable
#define maxLight 8
#define maxBones 64

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
	sampler2D texture;
	
#if defined(NORMAL_MAPPING)
	sampler2D normal;
#endif

#if defined(SPECULAR)
	sampler2D specular_tex;
#endif

#if defined(EMISSION)
	sampler2D emission_text;
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
	vec4  position;
	vec2  texcoord;
	vec3  normal;
	vec3  lightDir[maxLight];
	vec3  lightDirTBN[maxLight];
	vec3  viewDirTBN;
	vec3  viewDir;
	vec4  smcoord[maxLight];
	vec3 t;
	vec3 b;	
} Vert;

#if defined(SKINNING)
uniform int skinning_transformsNum;
uniform mat4 skinning_transforms[maxBones];
#endif 

uniform int lightsNum;
uniform vec4 light_position[maxLight];
uniform vec4 light_ambient[maxLight];
uniform vec4 light_diffuse[maxLight];
uniform vec4 light_specular[maxLight];
uniform vec3 light_attenuation[maxLight];
uniform vec3 light_spotDirection[maxLight];
uniform float light_spotExponent[maxLight];
uniform float light_spotCosCutoff[maxLight];
uniform mat4 light_transform[maxLight];
uniform int light_type[maxLight];
//uniform sampler2DShadow light_depthTexture[maxLight];
uniform sampler2D light_depthTexture[maxLight];

#if defined(VERTEX)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;
#if defined(SKINNING)
layout(location = 4) in vec4 bone_indices;
layout(location = 5) in vec4 bone_weights;
#endif

void ProcessLight(int i, vec4 vertex, vec3 t, vec3 b, vec3 n)
{
	Vert.smcoord[i] = light_transform[i] * vertex;
	Vert.smcoord[i].xyz /= Vert.smcoord[i].w;
	
	Vert.smcoord[i].w = light_position[i].w;
	Vert.smcoord[i].z = light_ambient[i].w;
	
	vec3 lightDir = light_position[i].xyz - vertex.xyz;
	
	Vert.lightDir[i] = lightDir;

	Vert.lightDirTBN[i].x = dot(lightDir, t);
	Vert.lightDirTBN[i].y = dot(lightDir, b);
	Vert.lightDirTBN[i].z = dot(lightDir, n);
}

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
		
	Vert.texcoord = texcoord;
	Vert.position = vertex;		
	
	vec3 n = transform.normal * normal;
	Vert.b = transform.normal * binormal;
	Vert.t = (cross(n, Vert.b));

#if defined(SKINNING)
	Vert.normal = transform.normal * MVIN * normal;
#else
	Vert.normal = transform.normal * normal;
#endif

	Vert.viewDir = normalize(vec3(transform.viewPosition - vec3(vertex)));
	
	Vert.viewDirTBN.x = dot(Vert.viewDir, Vert.t);
	Vert.viewDirTBN.y = dot(Vert.viewDir, Vert.b);
	Vert.viewDirTBN.z = dot(Vert.viewDir, n);

    for(int i = 0; i < min(maxLight, lightsNum); i++)
		ProcessLight(i, vertex, Vert.t, Vert.b, n);
	
    gl_Position = transform.viewProjection * (vertex);	
}

#elif defined(FRAGMENT)

float linstep(float low, float high, float v)
{
    return clamp((v-low)/(high-low), 0.0, 1.0);
}

float ChebyshevUpperBound(vec4 smcoord, sampler2D depthTexture, float distance, float bias)
{
	float compare = (distance - smcoord.z)/(smcoord.w - smcoord.z);
	vec2 moments = texture2D(depthTexture, smcoord.xy).rg;
	
	float p = smoothstep(compare-0.00001, compare, moments.x);
    float variance = max(moments.y - moments.x*moments.x, -0.001);
    float d = compare - moments.x + bias;
	
	float p_max = linstep(0.8, 1.0, variance / (variance + d*d));
		
	vec2 t = abs((smcoord.xy - 0.5) * 2); //mix(0, 1, );
		
	return mix(clamp(max(p, p_max), 0.0, 1.0), 1, pow(max(max(t.x,t.y), compare), 10));
}

out vec4 color;

float SampleShadow(in vec4 smcoord, sampler2D depthTexture, float distance, float bias)
{
#if defined(SHADOWS_PCF)
	float res = 0.0;

	res += textureProjOffset(depthTexture, smcoord, ivec2(-1,-1));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 0,-1));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 1,-1));
	res += textureProjOffset(depthTexture, smcoord, ivec2(-1, 0));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 0, 0));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 1, 0));
	res += textureProjOffset(depthTexture, smcoord, ivec2(-1, 1));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 0, 1));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 1, 1));

	return (res / 9.0);
#else
	return ChebyshevUpperBound(smcoord, depthTexture, distance, bias);
#endif
}

#if defined(SPECULAR)
vec3 ProccessLight(int i, vec4 diffuse, vec3 bump, vec4 specular, vec3 viewDir)
#else
vec3 ProccessLight(int i, vec4 diffuse, vec3 bump, vec3 viewDir)
#endif
{
	vec3 res = vec3(1);
	
	vec3 lightDir = vec3(0); 
	float distance = length(Vert.lightDir[i]);
	 
	float attenuation =  1 / (light_attenuation[i].x +
		light_attenuation[i].y * distance +
		light_attenuation[i].z * distance * distance);
		
	if(light_type[i] == 2 || light_type[i] == 3)
	{	
		//spot light
		lightDir = normalize(Vert.lightDir[i]);
		
		float spotEffect = dot(normalize(light_spotDirection[i]), -lightDir);
		float spot       = float(spotEffect > light_spotCosCutoff[i]);
		spotEffect = max(pow(spotEffect, light_spotExponent[i]), 0.0);		
		
		attenuation *= spotEffect * spotEffect;
	}
	else if(light_type[i] == 0 || light_type[i] == 1)
	{
		//directional light
		lightDir = normalize(-light_spotDirection[i]);		
	}	
	else
	{
		//point light
		lightDir = normalize(Vert.lightDir[i]);
	}
	
	res = light_ambient[i].xyz;
	
	float rawNdotL = dot(bump, lightDir);
	float NdotL = max(rawNdotL, 0);
	
	float shadow = 1;
	if(rawNdotL > 0)
	{
		res += light_diffuse[i].xyz * NdotL;
		res *= diffuse.xyz;
		
#if defined(SHADOW_RECEIVER)
		if(light_type[i] % 2 == 1)
			shadow = clamp(SampleShadow(Vert.smcoord[i], light_depthTexture[i], distance, NdotL * 0.01), 0.0, 1.0);
#endif		

#if defined(SPECULAR)
			float RdotVpow = max(pow(dot(reflect(-lightDir, bump), viewDir), material.shininess), 0.0);
			res += specular.xyz * material.specular.xyz * light_specular[i].xyz * RdotVpow;
#endif

	}
	
	res *= shadow * attenuation;
	
	return res;
}

void main(void)
{
	vec3 normal = Vert.normal;
	mat3 tbn = transpose(mat3((Vert.t), (Vert.b), (Vert.normal)));
	
#ifdef NORMAL_MAPPING
	normal = texture(material.normal, Vert.texcoord).xyz * 2.0 - 1.0;  
	normal *= tbn;	
#endif
    
	normal = normalize(normal);
	vec3 viewDir = normalize(Vert.viewDir);	
	
	vec4 diffuse = material.diffuse * texture(material.texture, Vert.texcoord);
	
#if defined(SPECULAR)
	vec4 specular = texture(material.specular_tex, Vert.texcoord);	
#endif
	
	vec3 res = vec3(0.0);
    
	for(int i = 0; i < lightsNum; i++)
#if defined(SPECULAR)
		res += ProccessLight(i, diffuse, normal, specular, viewDir);
#else
		res += ProccessLight(i, diffuse, normal, viewDir);
#endif

	color.xyz = material.emission.xyz;
	
#if defined(REFLECTION_CUBEMAP)	
	vec3 reflectDir = normalize(reflect(viewDir, normal));
	vec4 reflectColor = vec4(textureCube(material.cubemap, reflectDir)) * pow(1 - dot(Vert.normal, viewDir), 3);
	color.xyz += reflectColor.xyz;
#endif

#if defined(EMISSION)
	color.xyz += texture(material.emission_text, Vert.texcoord).xyz;
#endif	
	
	color.xyz += res;
    color.a = diffuse.a;
}

#endif
