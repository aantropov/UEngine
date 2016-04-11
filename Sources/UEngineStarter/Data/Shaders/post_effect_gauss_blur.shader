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

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main(void)
{
	color = texture( colorScene, vec2(gl_FragCoord)/512.0 ) * weight[0];
    for (int i=1; i<3; i++) {
        color +=
            texture( colorScene, ( vec2(gl_FragCoord)+vec2(0.0, offset[i]) )/512.0 )
                * weight[i];
        color +=
            texture( colorScene, ( vec2(gl_FragCoord)-vec2(0.0, offset[i]) )/512.0 )
                * weight[i];
    }
}

#endif