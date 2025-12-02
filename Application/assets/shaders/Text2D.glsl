#type vertex
#version 450 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_texcoord;


layout(location = 2) in vec2 a_int_size;
layout(location = 3) in vec2 a_int_pos;
layout(location = 4) in vec2 a_int_texture_size;
layout(location = 5) in vec4 a_int_texture_rect;
layout(location = 6) in vec4 a_int_color;
layout(location = 7) in int a_int_mode;

uniform mat3 u_view_matrix;
uniform mat3 u_screen_matrix;

out vec2 tex_coord;
out vec4 text_color;

void main()
{
 text_color = a_int_color; 

  

  vec2 local_pos = a_pos * a_int_size;
  
  
  local_pos += a_int_pos;


  vec3 pos = vec3(local_pos, 1.0);

  vec3 ndc;
  if(a_int_mode == 0)
  {
    ndc = u_screen_matrix * pos;
  }
  else if(a_int_mode == 1)
  {
    ndc = u_view_matrix * pos;
  }

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
in vec4 text_color;

uniform sampler2D u_texture;
uniform vec2 u_scale;

float median(float r, float g, float b) 
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    float scale = (u_scale.x + u_scale.y) * 0.5;
    float u_smoothing = 0.01;
    vec3 s = texture(u_texture, tex_coord).rgb;
    float sd = median(s.r, s.g, s.b);
    float alpha = smoothstep(0.5 - u_smoothing, 0.5 + u_smoothing, sd);
    FragColor = vec4(text_color.rgb, text_color.a * alpha);
}

