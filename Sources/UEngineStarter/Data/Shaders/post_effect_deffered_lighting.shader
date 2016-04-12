#define maxLight 8
#extension GL_ARB_texture_rectangle: enable

precision highp float;

#if defined(VERTEX)
	#define inout out
#elif defined(FRAGMENT)
	#define inout in
#endif

uniform int lightsNum;
uniform float lightIndex;
uniform float state;

uniform sampler2D colorScene;
uniform sampler2D depthScene;
uniform sampler2D normalScene;
uniform sampler2D diffuseScene;
uniform sampler2D ambientScene;
uniform sampler2D specularScene;
uniform sampler2D positionScene;
uniform sampler2D previousScene;

uniform struct Transform
{
	mat4 view;
	mat4 model;
	vec3 viewPosition;
	mat4 viewProjectionInv;
} transform;

uniform vec4 light_position[maxLight];
uniform vec4 light_ambient[maxLight];
uniform vec4 light_diffuse[maxLight];
uniform vec4 light_specular[maxLight];
uniform vec3 light_attenuation[maxLight];
uniform mat4 light_transform[maxLight];
uniform vec3 light_spotDirection[maxLight];
uniform float light_spotExponent[maxLight];
uniform float light_spotCosCutoff[maxLight];

inout Vertex
{
  vec2 texcoord;
  highp vec3 position;
  highp vec3 frustumFarPlane;
  highp vec3 frustumNearPlane;
} Vert;

uniform struct Camera
{
   highp float zFar;
   highp float zNear;
} camera;

#if defined(VERTEX)

layout(location = 0) in highp vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 texcoord;

void main(void)
{
  Vert.texcoord = texcoord;
  Vert.position = position;
    
  vec4 frustumFar = transform.viewProjectionInv * vec4(position.xy, 1.0f, 1.0f);
  Vert.frustumFarPlane = frustumFar.xyz / frustumFar.w;
  
  vec4 frustumNear = transform.viewProjectionInv * vec4(position.xy, 0.0f, 1.0f);
  Vert.frustumNearPlane = frustumNear.xyz / frustumNear.w;
  
  gl_Position = vec4(position, 1.0f);
}

#elif defined(FRAGMENT)

#extension GL_ARB_texture_rectangle: enable

out vec4 color;

vec3 GetWorld(float zEye)
{	
	return transform.viewPosition + (Vert.frustumFarPlane - Vert.frustumNearPlane) * zEye;	
}

vec3 GetWorldFromDepth()
{	
    float depth = texture(depthScene, Vert.texcoord).r* 2.0 - 1.0;
	
	vec4 pos;
	pos.xy = Vert.texcoord * 2.0 - 1.0;
	pos.z = depth;
	pos.w = 1.0;
	
	pos = transform.viewProjectionInv * pos;
	pos /= pos.w;

    return pos.xyz;
}

vec4 ProccessLight(int i, vec3 bump, vec4 vertPosition, vec4 diffuse, vec4 specular)
{
	vec4 res = vec4(0);

	vec3 viewDir = normalize(transform.viewPosition - vertPosition.xyz);
	vec3 lightDir = light_position[i].xyz - vertPosition.xyz;
	float distance = length(lightDir);
	lightDir = normalize(lightDir);
	
	float spotEffect = dot(normalize(light_spotDirection[i]), -lightDir);
	float spot       = float(spotEffect > light_spotCosCutoff[i]);
	spotEffect = max(pow(spotEffect, light_spotExponent[i]), 0.0);
	
	float attenuation = spot * spotEffect / (light_attenuation[i].x +
		light_attenuation[i].y * distance +
		light_attenuation[i].z * distance * distance);
	
	res = light_ambient[i];
	
	float rawNdotL = dot(bump, lightDir);
	float NdotL = max(rawNdotL, 0);
		
	if(rawNdotL > 0)
	{
		res += light_diffuse[i] * NdotL;
		res *= diffuse;
		
		float RdotVpow = max(pow(dot(reflect(normalize(vertPosition.xyz - light_position[i].xyz), bump), viewDir), specular.w * 255.0f), 0.0);
		res += vec4(specular.xyz * light_specular[i].xyz, 1.0) * RdotVpow;
		
		res *= attenuation;
	}
	
	return res;
}

void main(void)
{
  vec4 vertPosition  = vec4(GetWorld(texture(positionScene, Vert.texcoord).r), 1.0f);
  
  if(vertPosition.z > camera.zFar)
	discard;
	
  vec3 vertNormal  = (texture(normalScene, Vert.texcoord).xyz * 2.0 - vec3(1.0));
  //vertNormal.z = sqrt(1 - (vertNormal.x * vertNormal.x + vertNormal.y * vertNormal.y));
  
  vec4 diffuse = texture(diffuseScene, Vert.texcoord);
  vec4 specular = texture(specularScene, Vert.texcoord);  
  vec4 previous = texture(previousScene, Vert.texcoord);
  vec3 emission  = texture(colorScene, Vert.texcoord).xyz;  
	
  vec4 res = ProccessLight(int(lightIndex), vertNormal, vertPosition, diffuse, specular);

  color = vec4(0);
  
  if(int(state) == 0)
    color = res;
  
  if(int(state) == 1)
    color = res + previous;
  
  if(int(state) == 2)
	color = (res + previous) + vec4(emission, 1.0);

  if(int(state) == 3)
    color = res + vec4(emission, 1.0);	
}

#endif