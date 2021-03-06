#if defined(VERTEX)
	#define inout out
#elif defined(FRAGMENT)
	#define inout in
#endif

uniform sampler2D noise;
uniform sampler2D depthScene;
uniform sampler2D colorScene;
uniform sampler2D normalScene;

uniform float time;

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

float LinearizeDepth(float zoverw)
{
	float n =  0.001f; // camera z near
	float f = 1000.0; // camera z far
	return 45 * (2.0 * n) / (f + n - zoverw * (f - n));
}

void main(void)
{	

	float zb = (texture (depthScene, Vert.texcoord ).x);
	
	const float zFar      = 100.0;
    const float zNear     = 0.01;
    const float attScale  = 1.0;
    const float distScale = 0.5;
	const float radius    = 0.2;
	const float bias      = 100.0;
	
	color = vec4(1);
	if( zb*zFar <  99.997)
	{
		vec4	rndTable [8] = vec4 [8] 
		(
			vec4 ( -0.5, -0.5, -0.5, 0.0 ),
			vec4 (  0.5, -0.5, -0.5, 0.0 ),
			vec4 ( -0.5,  0.5, -0.5, 0.0 ),
			vec4 (  0.5,  0.5, -0.5, 0.0 ),
			vec4 ( -0.5, -0.5,  0.5, 0.0 ),
			vec4 (  0.5, -0.5,  0.5, 0.0 ),
			vec4 ( -0.5,  0.5,  0.5, 0.0 ),
			vec4 (  0.5,  0.5,  0.5, 0.0 )
		);
    
    
		float z = zFar*zNear / (zb * (zFar - zNear) - zFar);   
		vec3 pe = Vert.position * z / Vert.position;
		float att = 0.0;
		vec3 plane = 2.0 * texture(noise,  Vert.texcoord * 512.0/4.0).xyz - vec3 (1.0);
    
		for(int i = 0; i < 8; i++)
		{
			vec3 sample = reflect(rndTable[i].xyz, plane );
			float zSample = texture2D(depthScene, Vert.texcoord + radius*sample.xy / z).x;

			zSample = zFar * zNear / (zSample * (zFar - zNear) - zFar );
        
			float dist = max ( zSample - z, 0.0 ) / distScale;    
			float occl = 15 * max ( dist * (2.0 - dist), 0.0 );
        
			att += 1.0/(1.0 + occl*occl);
		}
    
		att = clamp(att/8.0 + 0.45, 0.0, 1.0) * attScale;
		color = vec4(att);
	}
	
	color *= texture(colorScene, Vert.texcoord);	
}
#endif
