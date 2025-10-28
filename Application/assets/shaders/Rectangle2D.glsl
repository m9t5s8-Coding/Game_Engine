#type vertex
#version 450 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec4 a_fill_color;


uniform mat3 u_view_matrix;

out vec4 vertex_color;


void main()
{
  vertex_color = a_fill_color;
  vec3 pos = vec3(a_pos, 1.0);
  vec3 ndc = u_view_matrix * pos;
  gl_Position = vec4(ndc.xy, 0.0, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;

in vec4 vertex_color;

void main()
{
  FragColor = vertex_color;
}
