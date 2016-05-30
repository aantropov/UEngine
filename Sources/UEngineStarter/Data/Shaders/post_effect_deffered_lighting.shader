#define maxLight 8
#extension GL_ARB_texture_rectangle: enable

precision highp float;

#if defined(VERTEX)
	#define inout out
#elif defined(FRAGMENT)
	#define inout in
#endif

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

uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec3 light_attenuation;
uniform mat4 light_transform;
uniform vec3 light_spotDirection;
uniform float light_spotExponent;
uniform float light_spotCosCutoff;
uniform sampler2D light_depthTexture;
uniform int light_type;

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
	vec2 t = abs((smcoord.xy - 0.5) * 2);
		
	return mix(clamp(max(p, p_max), 0.0, 1.0), 1, pow(max(max(t.x,t.y), compare), 10));
}


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

vec4 ProccessLight(vec3 bump, vec4 vertPosition, vec4 diffuse, vec4 specular, float shadow_receiver)
{
	vec4 res = vec4(0);

	vec3 viewDir = normalize(transform.viewPosition - vertPosition.xyz);
	vec3 lightDir = light_position.xyz - vertPosition.xyz;
	float distance = length(lightDir);
	lightDir = normalize(lightDir);
	
	float attenuation = 1 / (light_attenuation.x +
		light_attenuation.y * distance +
		light_attenuation.z * distance * distance);
	
	if(light_type == 2 || light_type == 3)
	{
		//spot light
		float spotEffect = dot(normalize(light_spotDirection), -lightDir);
		float spot       = float(spotEffect > light_spotCosCutoff);
		spotEffect = max(pow(spotEffect, light_spotExponent), 0.0);
		
		attenuation *= spotEffect * spotEffect;
	}
	else if(light_type == 0 || light_type == 1)
	{
		//directional light
		lightDir = normalize(-light_spotDirection);		
	}
	
	res = light_ambient;
	
	float rawNdotL = dot(bump, lightDir);
	float NdotL = max(rawNdotL, 0);
	
	float shadow = 1;
	if(rawNdotL > 0)
	{
		res += light_diffuse * NdotL;
		res *= diffuse;
		
		if(light_type % 2 == 1 && shadow_receiver == 1)
		{
			vec4 smcoord = light_transform * vertPosition;
			smcoord.xyz /= smcoord.w;
			smcoord.w = light_position.w;
			smcoord.z = light_ambient.w;
			
			shadow = clamp(SampleShadow(smcoord, light_depthTexture, distance, NdotL*0.01), 0.0, 1.0);
		}
		
		//shadow *= spot * spotEffect;
		
		float RdotVpow = max(pow(dot(reflect(normalize(vertPosition.xyz - light_position.xyz), bump), viewDir), specular.w * 255.0f), 0.0);
		res += vec4(specular.xyz * light_specular.xyz, 1.0) * RdotVpow;
		
		res *= shadow * attenuation;
	}
	
	return res;
}

void main(void)
{
  vec4 vertPosition  = vec4(GetWorld(texture(positionScene, Vert.texcoord).r), 1.0f);
  
  if(vertPosition.z > camera.zFar)
	discard;

  vec4 normal_tex = texture(normalScene, Vert.texcoord);
  vec3 vertNormal  = (normal_tex.xyz * 2.0 - vec3(1.0));
  //vertNormal.z = sqrt(1 - (vertNormal.x * vertNormal.x + vertNormal.y * vertNormal.y));
  
  vec4 diffuse = texture(diffuseScene, Vert.texcoord);
  vec4 specular = texture(specularScene, Vert.texcoord);  
  vec4 previous = texture(previousScene, Vert.texcoord);
  vec4 emission  = texture(colorScene, Vert.texcoord);  
    
  vec4 res = ProccessLight(vertNormal, vertPosition, diffuse, specular, normal_tex.w);

  color = vec4(0); 
  
  if(int(state) == 0)
    color = res;
  
  if(int(state) == 1)
    color = res + previous;
  
  if(int(state) == 2)
	color = (res + previous) + emission;

  if(int(state) == 3)
    color = res + emission;	
	
  //color = vec4(emission);
}

#endif