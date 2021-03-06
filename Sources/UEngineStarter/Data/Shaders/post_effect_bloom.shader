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
uniform sampler2D bluredScene;
uniform float power;
uniform float border;

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

float Luminance(vec3 color)
{
	return (color.g + color.r + color.b) / 3;
}

void main(void)
{
  vec4 blured = texture(bluredScene, Vert.texcoord);
  blured *= pow(Luminance(blured.rgb), 2) * 1;
  
  color = texture(colorScene, Vert.texcoord) + blured;  
}

#endif