#type vertex
#version 450 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_texcoord;

uniform mat3 u_view_matrix;

out vec2 texture_coord;

void main()
{
  texture_coord = a_texcoord;

  vec3 pos = vec3(a_pos, 1.0);
  vec3 ndc = u_view_matrix * pos;
  gl_Position = vec4(ndc.xy, 0.0, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;

in vec2 texture_coord;

uniform sampler2D u_texture;

void main()
{
  FragColor = texture(u_texture, texture_coord);
}
