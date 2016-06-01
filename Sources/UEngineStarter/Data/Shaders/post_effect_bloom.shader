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
  vec4 masked = texture(bluredScene, Vert.texcoord);//(textureLod(colorScene, Vert.texcoord, 0) + textureLod(colorScene,Vert.texcoord, 1) + textureLod(colorScene,Vert.texcoord, 2)) * 0.33f;
  //color = texture(colorScene, Vert.texcoord) + vec4(1,1,1,1) * clamp(0, 1, Luminance(masked.rgb) );  
  color = texture(colorScene, Vert.texcoord) + masked * mix(0, 1, pow(Luminance(masked.rgb) - 0.1, 2)); 
}

#endif