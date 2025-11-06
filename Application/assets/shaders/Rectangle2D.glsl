#type vertex
#version 450 core

layout(location = 0) in vec2 a_pos;

layout(location = 1) in vec2 a_int_size;
layout(location = 2) in vec2 a_int_pos;
layout(location = 3) in vec2 a_int_origin;
layout(location = 4) in float a_int_rotation;
layout(location = 5) in float a_int_border_thickness;
layout(location = 6) in vec4 a_int_fill_color;
layout(location = 7) in vec4 a_int_border_color;

uniform mat3 u_view_matrix;

out vec4 fill_color;
out vec4 border_color;
out vec2 frag_pos;
out vec2 size;

out float border_size;


void main()
{
  fill_color = a_int_fill_color;
  border_color = a_int_border_color;

  vec2 total_size;
  total_size.x = max(a_int_size.x, a_int_size.x + (a_int_border_thickness * 2));
  total_size.y = max(a_int_size.y, a_int_size.y + (a_int_border_thickness * 2));
  size = total_size;
  border_size = abs(a_int_border_thickness);



  vec2 origin = vec2(total_size.x / 2, total_size.y / 2);

  float cosA = cos(a_int_rotation);
  float sinA = sin(a_int_rotation);

  vec2 local_pos = a_pos * total_size;
  frag_pos = local_pos;
  local_pos -= origin;
  
  vec2 rotated;
    rotated.x = local_pos.x * cosA - local_pos.y * sinA;
    rotated.y = local_pos.x * sinA + local_pos.y * cosA;

  rotated += a_int_pos;

  vec3 pos = vec3(rotated, 1.0);
  vec3 ndc = u_view_matrix * pos;
  gl_Position = vec4(ndc.xy, 0.0, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;

in vec4 fill_color;
in vec4 border_color;
in vec2 frag_pos;
in vec2 size;
in float border_size;

void main()
{
  bool is_border = frag_pos.x < border_size || frag_pos.x > (size.x - border_size) ||
                  frag_pos.y < border_size || frag_pos.y > (size.y - border_size);
  FragColor =  is_border ? border_color : fill_color;
}
