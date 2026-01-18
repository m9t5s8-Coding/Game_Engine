#type vertex
#version 450 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_texcoord;

out vec2 tex_coord;

void main()
{
  tex_coord = a_texcoord;
  gl_Position = vec4(a_pos, 0.0, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D u_texture;


void main()
{
  FragColor = texture(u_texture, tex_coord);
}
