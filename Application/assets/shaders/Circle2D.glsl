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
out vec2 center;

out float border_size;


void main()
{
  fill_color = a_int_fill_color;
  border_color = a_int_border_color;

  vec2 total_size;
  total_size.x = max(a_int_size.x, a_int_size.x + a_int_border_thickness * 2);
  total_size.y = max(a_int_size.y, a_int_size.y + a_int_border_thickness * 2);

  size = total_size;
  border_size = abs(a_int_border_thickness);



  vec2 origin = vec2(total_size.x / 2, total_size.y / 2);
  center = origin;

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
in vec2 center;
in float border_size;

void main()
{
  float radius_x = size.x / 2.0;
  float radius_y = size.y / 2.0;

  vec2 delta = frag_pos - center;
  float ellipse_dist = (delta.x * delta.x) / (radius_x * radius_x) +
                       (delta.y * delta.y) / (radius_y * radius_y);

  if (ellipse_dist > 1.0)
    discard;

  float inner_ellipse = (delta.x * delta.x) / pow(radius_x - border_size, 2.0) +
                        (delta.y * delta.y) / pow(radius_y - border_size, 2.0);

  FragColor = inner_ellipse > 1.0 ? border_color : fill_color;
}
