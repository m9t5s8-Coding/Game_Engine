#type vertex
#version 330 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texcoord;

out vec4 vertex_color;

void main()
{
  vertex_color = a_color;
  gl_Position = vec4(a_pos, 0.0, 1.0);
}

#type fragment
#version 330 core

out vec4 FragColor;

in vec4 vertex_color;

void main()
{
  FragColor = vertex_color;
}
