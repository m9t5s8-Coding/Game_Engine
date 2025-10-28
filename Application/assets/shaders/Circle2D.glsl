#type vertex
#version 450 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_center;
layout(location = 2) in vec2 a_radius;
layout(location = 3) in vec4 a_fill_color;

uniform mat3 u_view_matrix;

out vec4 vertex_color;
out vec2 circle_center;
out vec2 frag_pos;
out vec2 circle_radius;

void main()
{
  vertex_color = a_fill_color;
  circle_center = a_center;
  circle_radius = a_radius;

  frag_pos = a_pos;
  
  vec3 pos = vec3(a_pos, 1.0);
  vec3 ndc = u_view_matrix * pos;
  gl_Position = vec4(ndc.xy, 0.0, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;

in vec4 vertex_color;
in vec2 circle_center;
in vec2 frag_pos;
in vec2 circle_radius;

void main()
{
  vec2 offset = frag_pos - circle_center;
  float eq = (offset.x * offset.x) / (circle_radius.x * circle_radius.x)
             + (offset.y * offset.y) / (circle_radius.y * circle_radius.y);

  if( eq <= 1)
    FragColor = vertex_color;
  else
    discard;
}
