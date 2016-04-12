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
uniform float time;
uniform float blurAmount;

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

uniform float offset[3] = float[]( 0.0, 0.0013846153846, 0.0032307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main(void)
{
	color = texture(colorScene, Vert.texcoord ) * weight[0] * 2;
    for (int i=1; i<3; i++) {
        color += texture(colorScene, (Vert.texcoord+vec2(0.0, offset[i] * blurAmount)))* weight[i];
        color += texture(colorScene, (Vert.texcoord-vec2(0.0, offset[i] * blurAmount)))* weight[i];
		color += texture(colorScene, (Vert.texcoord+vec2(offset[i] * blurAmount, 0.0)))* weight[i];
        color += texture(colorScene, (Vert.texcoord-vec2(offset[i] * blurAmount, 0.0)))* weight[i];
    }	
	color /= 2;
}

#endif