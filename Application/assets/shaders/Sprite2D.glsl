#type vertex
#version 450 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_texcoord;


layout(location = 2) in vec2 a_int_size;
layout(location = 3) in vec2 a_int_pos;
layout(location = 4) in float a_int_rotation;
layout(location = 5) in vec2 a_int_texture_size;
layout(location = 6) in vec4 a_int_texture_rect;

uniform mat3 u_view_matrix;

out vec2 tex_coord;

void main()
{
  
  vec2 origin = vec2(a_int_size.x / 2, a_int_size.y / 2);

  float cosA = cos(a_int_rotation);
  float sinA = sin(a_int_rotation);

  vec2 local_pos = a_pos * a_int_size;
  local_pos -= origin;
  
  vec2 rotated;
    rotated.x = local_pos.x * cosA - local_pos.y * sinA;
    rotated.y = local_pos.x * sinA + local_pos.y * cosA;

  rotated += a_int_pos;


  vec3 pos = vec3(rotated, 1.0);
  vec3 ndc = u_view_matrix * pos;
  gl_Position = vec4(ndc.xy, 0.0, 1.0);

  vec2 tex_local_pos;
  tex_local_pos.x = a_texcoord.x * a_int_texture_rect.z + a_int_texture_rect.x;
  tex_local_pos.y = a_texcoord.y * a_int_texture_rect.w + a_int_texture_rect.y;

  float ndc_tex_x = tex_local_pos.x / a_int_texture_size.x;
  float ndc_tex_y = tex_local_pos.y / a_int_texture_size.y;

  tex_coord = vec2(ndc_tex_x,ndc_tex_y);
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
