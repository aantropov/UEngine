#if defined(VERTEX)
	#define inout out
#elif defined(FRAGMENT)
	#define inout in
#endif

inout Vertex
{
  vec2 texcoord;
  vec3 position;
} Vert;

uniform sampler2D colorScene;
uniform sampler2D depthScene;
uniform float time;

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

vec3 filter(in vec2 texcoord)
{
	vec2 center = vec2(0.5, 0.5);
	float d = dot(texcoord.xy - center,texcoord.xy - center)*5.0;
	float factor = sin(texcoord.x * 50 + time / 300) * sin(texcoord.y * 50 + time / 200) * 0.001;
	
	return vec3(0.33 * (texture(colorScene, texcoord).rgb + 
					   texture(colorScene, texcoord.xy + d*factor).rgb + 
					   texture(colorScene, texcoord.xy - d*factor).rgb
					   ));
	
}

void main(void)
{
  vec3 res = filter(Vert.texcoord); 
  color = vec4(res, 1.0);
}

#endif