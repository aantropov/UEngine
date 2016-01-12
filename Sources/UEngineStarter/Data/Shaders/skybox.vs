#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 binormal;
layout(location = 3) in vec2 texcoord;

out 	vec3 	v;
out 	vec3 	l;
out 	vec3 	pos;
out 	vec3 	colorYxy;

//uniform	vec4 	sunPos;
//uniform	vec4 	eyePos;
uniform	float	turbidity;

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;	
	mat3 normal;
	vec3 viewPosition;
        mat4 modelViewProjection;
} transform;


out Vertex 
{
	vec4  position;
	vec2  texcoord;
}Vert;

//
// Perez zenith func
//

vec3 perezZenith ( float t, float thetaSun )
{
	const float	pi = 3.1415926;
	const vec4	cx1 = vec4 ( 0,       0.00209, -0.00375, 0.00165  );
	const vec4	cx2 = vec4 ( 0.00394, -0.03202, 0.06377, -0.02903 );
	const vec4	cx3 = vec4 ( 0.25886, 0.06052, -0.21196, 0.11693  );
	const vec4	cy1 = vec4 ( 0.0,     0.00317, -0.00610, 0.00275  );
	const vec4	cy2 = vec4 ( 0.00516, -0.04153, 0.08970, -0.04214 );
	const vec4	cy3 = vec4 ( 0.26688, 0.06670, -0.26756, 0.15346  );

	float	t2    = t*t;
	float	chi   = (4.0 / 9.0 - t / 120.0 ) * (pi - 2.0 * thetaSun );
	vec4	theta = vec4 ( 1, thetaSun, thetaSun*thetaSun, thetaSun*thetaSun*thetaSun );

	float	Y = (4.0453 * t - 4.9710) * tan ( chi ) - 0.2155 * t + 2.4192;
	float	x = t2 * dot ( cx1, theta ) + t * dot ( cx2, theta ) + dot ( cx3, theta );
	float	y = t2 * dot ( cy1, theta ) + t * dot ( cy2, theta ) + dot ( cy3, theta );

	return vec3 ( Y, x, y );
}

//
// Perez allweather func (turbidity, cosTheta, cosGamma)
//

vec3  perezFunc ( float t, float cosTheta, float cosGamma )
{
    float  gamma      = acos ( cosGamma );
    float  cosGammaSq = cosGamma * cosGamma;
    float  aY =  0.17872 * t - 1.46303;
    float  bY = -0.35540 * t + 0.42749;
    float  cY = -0.02266 * t + 5.32505;
    float  dY =  0.12064 * t - 2.57705;
    float  eY = -0.06696 * t + 0.37027;
    float  ax = -0.01925 * t - 0.25922;
    float  bx = -0.06651 * t + 0.00081;
    float  cx = -0.00041 * t + 0.21247;
    float  dx = -0.06409 * t - 0.89887;
    float  ex = -0.00325 * t + 0.04517;
    float  ay = -0.01669 * t - 0.26078;
    float  by = -0.09495 * t + 0.00921;
    float  cy = -0.00792 * t + 0.21023;
    float  dy = -0.04405 * t - 1.65369;
    float  ey = -0.01092 * t + 0.05291;

    return vec3 ( (1.0 + aY * exp(bY/cosTheta)) * (1.0 + cY * exp(dY * gamma) + eY*cosGammaSq),
                  (1.0 + ax * exp(bx/cosTheta)) * (1.0 + cx * exp(dx * gamma) + ex*cosGammaSq),
                  (1.0 + ay * exp(by/cosTheta)) * (1.0 + cy * exp(dy * gamma) + ey*cosGammaSq) );
}

vec3  perezSky ( float t, float cosTheta, float cosGamma, float cosThetaSun )
{
    float thetaSun = acos        ( cosThetaSun );
    vec3  zenith   = perezZenith ( t, thetaSun );
    vec3  clrYxy   = zenith * perezFunc ( t, cosTheta, cosGamma ) / perezFunc ( t, 1.0, cosThetaSun );

    clrYxy [0] *= smoothstep ( 0.0, 0.1, cosThetaSun );			// make sure when thetaSun > PI/2 we have black color
	
    return clrYxy;
}

void main(void)
{
	
	vec4 vertex = transform.model * vec4(position, 1.0);
	vec4 sunPos = vec4(1.0, 1.0, 0.0, 0.0);
	
	Vert.texcoord = texcoord;
	Vert.position = vertex;

    v               = normalize ( vertex.xyz - transform.viewPosition );
    l               = normalize ( sunPos.xyz );
    pos             = 0.1 * vertex.xyz;
    colorYxy        = perezSky ( turbidity, max ( v.y, 0.0 ) + 0.05, dot ( l, v ), l.y );

    gl_Position = transform.viewProjection * (vertex);	
}