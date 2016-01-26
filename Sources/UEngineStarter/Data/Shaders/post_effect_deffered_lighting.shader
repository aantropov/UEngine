#define maxLight 8

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
	mat4 model;
	vec3 viewPosition;
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
  vec3 position;
} Vert;


#if defined(VERTEX)

layout(location = 0) in vec3 position;
layout(location = 3) in vec2 texcoord;

void main(void)
{
  Vert.texcoord = texcoord;
  Vert.position = position;
  gl_Position = vec4(position, 1.0);
}

#elif defined(FRAGMENT)

out vec4 color;

vec4 ProccessLight(int i, vec3 bump, vec4 vertPosition, vec4 ambient, vec4 diffuse, vec4 specular)
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
		
	res = ambient * light_ambient[i];
	    
	float NdotL = max(dot(bump, lightDir), 0);
	res += diffuse * light_diffuse[i] * NdotL;
    
	float RdotVpow = max(pow(dot(reflect(normalize(vertPosition.xyz - light_position[i].xyz), bump), viewDir), specular.w * 255.0f), 0.0);
	res += vec4(specular.xyz * light_specular[i].xyz, 1.0) * RdotVpow;
	
	return res * attenuation;
}

void main(void)
{
  vec4 vertPosition  = texture(positionScene, Vert.texcoord);
  vec3 vertNormal  = (texture(normalScene, Vert.texcoord).xyz * 2.0 - vec3(1.0));
  vec4 ambient = texture(ambientScene, Vert.texcoord);
  vec4 diffuse = texture(diffuseScene, Vert.texcoord);
  vec4 specular = texture(specularScene, Vert.texcoord);  
  vec4 previous = texture(previousScene, Vert.texcoord);
  vec3 emission  = texture(colorScene, Vert.texcoord).xyz;  
	
  vec4 res = ProccessLight(int(lightIndex), vertNormal, vertPosition, ambient, diffuse, specular);
  //res = vec4(vertNormal,1);
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