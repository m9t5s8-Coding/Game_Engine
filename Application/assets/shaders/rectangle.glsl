#type vertex
#version 330 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 u_view_projection;
uniform mat4 u_transform;

void main()
{
  gl_Position = u_view_projection * u_transform * vec4(a_pos, 0.0, 1.0);
}

#type fragment
#version 330 core

out vec4 FragColor;


void main()
{
  FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
