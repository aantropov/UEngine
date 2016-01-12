in vec3 position;

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;
	mat3 normal;
	vec3 viewPosition;
        mat4 modelViewProjection;
} transform;

void main(void)
{
    gl_Position = transform.modelViewProjection * vec4(position, 1.0);
}