
#type vertex
#version 300 es
precision highp float;

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_texcoord;

out vec2 tex_coord;
uniform vec2 u_size;
uniform vec2 u_position;
uniform mat3 u_screen_matrix;

void main()
{
    vec2 local = a_pos * u_size - u_size * 0.5 + u_position;

  vec3 world = vec3(local, 1.0);
  vec3 ndc   = u_screen_matrix * world;
  gl_Position = vec4(ndc.xy, 0.0, 1.0);

  tex_coord = a_texcoord;
}


#type fragment
#version 300 es
precision mediump float;

in vec2 tex_coord;

uniform sampler2D u_texture;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_texture, tex_coord);
}
