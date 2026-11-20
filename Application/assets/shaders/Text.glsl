#type vertex
#version 450 core

// Dedicated MSDF text shader used by the Independent text render path.
// Instances use the same buffer layout as Quad.glsl so the same vertex array
// can be reused, but only the fields meaningful for glyphs are consumed here.

layout(location = 0) in vec2 a_pos;

layout(location = 1) in vec2  a_size;
layout(location = 2) in vec2  a_position;
layout(location = 3) in vec2  a_origin;
layout(location = 4) in float a_rotation;
layout(location = 5) in int   a_mode;
layout(location = 6) in int   a_type;
layout(location = 7) in int   a_slot;

layout(location = 8)  in float a_border_thickness;
layout(location = 9)  in vec4  a_fill_color;
layout(location = 10) in vec4  a_border_color;

layout(location = 11) in float a_corner_radius;

layout(location = 12) in vec2  a_texture_size;
layout(location = 13) in vec4  a_texture_rect;
layout(location = 14) in vec2  a_flip;
layout(location = 15) in int   a_entity_id;

uniform mat3 u_view_matrix;
uniform mat3 u_screen_matrix;

out vec4 fill_color;
out vec2 tex_coord;

flat out int v_entity_id;

void main()
{
    fill_color  = a_fill_color;
    v_entity_id = a_entity_id;

    // a_position is the glyph quad centre and a_origin is its pivot (half the
    // glyph size on the CPU). Rotate around the pivot so text follows the
    // transform rotation just like sprites and rectangles.
    vec2 local = a_pos * a_size - a_origin;
    float c = cos(a_rotation);
    float s = sin(a_rotation);
    vec2 rotated = vec2(local.x * c - local.y * s,
                        local.x * s + local.y * c) + a_position;

    vec3 world = vec3(rotated, 1.0);
    vec3 ndc = (a_mode == 0) ? u_screen_matrix * world : u_view_matrix * world;
    gl_Position = vec4(ndc.xy, 0.0, 1.0);

    // a_texture_rect is already inset by half a texel on the CPU, which keeps
    // linear filtering from sampling a neighbouring glyph in the atlas.
    vec2 tex_size = max(a_texture_size, vec2(1.0));
    tex_coord = (a_texture_rect.xy + a_pos * a_texture_rect.zw) / tex_size;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int entity_id;

in vec4 fill_color;
in vec2 tex_coord;

flat in int v_entity_id;

// Keep the same sampler-array contract as Quad.glsl so Renderer::submit_instanced
// can upload the sampler slots for this shader too. Text always samples slot 0.
uniform sampler2D u_textures[2];

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    vec3  msdf = texture(u_textures[0], tex_coord).rgb;
    float sd   = median(msdf.r, msdf.g, msdf.b);

    // Screen-space derivative keeps the SDF edge one pixel wide at any font
    // size instead of using a fixed threshold range.
    float smoothing = max(fwidth(sd), 0.0001);
    float alpha     = smoothstep(0.5 - smoothing, 0.5 + smoothing, sd);

    if (alpha <= 0.001)
        discard;

    FragColor = vec4(fill_color.rgb, fill_color.a * alpha);
    entity_id = v_entity_id;
}
