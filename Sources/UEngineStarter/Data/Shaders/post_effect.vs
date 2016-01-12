#version 330 core

layout(location = 0) in vec3 position;
layout(location = 3) in vec2 texcoord;

out Vertex
{
  vec2 texcoord;
  vec3 position;
} Vert;

void main(void)
{
  Vert.texcoord = texcoord;
  Vert.position = position;
  gl_Position = vec4(position, 1.0);
}