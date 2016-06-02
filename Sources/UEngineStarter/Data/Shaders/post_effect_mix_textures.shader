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

uniform sampler2D texture1;
uniform float param1;
uniform sampler2D texture2;
uniform float param2;

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

void main(void)
{
	color = texture(texture1, Vert.texcoord) * param1 +  texture(texture2, Vert.texcoord) * param2;	
}

#endif