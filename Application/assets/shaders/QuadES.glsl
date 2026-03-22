
#type vertex
#version 300 es
precision highp float;
precision highp int;

// Per-vertex
layout(location = 0) in vec2 a_pos;

// Per-instance
layout(location = 1)  in vec2  a_size;
layout(location = 2)  in vec2  a_position;
layout(location = 3)  in vec2  a_origin;
layout(location = 4)  in float a_rotation;
layout(location = 5)  in int   a_mode;
layout(location = 6)  in int   a_type;
layout(location = 7)  in int   a_slot;

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

out vec4  fill_color;
out vec4  border_color;
out vec2  frag_pos;
out vec2  size;
out vec2  center;
out vec2  tex_coord;
out float border_size;
out float corner_radius;

flat out int v_type;
flat out int v_slot;
flat out int v_entity_id;

void main()
{
    fill_color    = a_fill_color;
    border_color  = a_border_color;
    corner_radius = a_corner_radius;
    border_size   = abs(a_border_thickness);
    v_type        = a_type;
    v_slot        = a_slot;
    v_entity_id   = a_entity_id;

    // Rectangle or Circle
    if (a_type == 0 || a_type == 1)
    {
        vec2 total_size = max(a_size, a_size + vec2(a_border_thickness * 2.0));
        size   = total_size;
        center = total_size * 0.5;

        vec2 local = a_pos * total_size - center;
        frag_pos   = a_pos * total_size;

        float c = cos(a_rotation);
        float s = sin(a_rotation);
        vec2 rotated = vec2(local.x * c - local.y * s,
                            local.x * s + local.y * c) + a_position;

        vec3 world = vec3(rotated, 1.0);
        vec3 ndc   = (a_mode == 0) ? u_screen_matrix * world : u_view_matrix * world;
        gl_Position = vec4(ndc.xy, 0.0, 1.0);
    }
    // Sprite
    else if (a_type == 2)
    {
        vec2 local = a_pos * a_size - a_size * 0.5;
        size     = a_size;
        frag_pos = a_pos * a_size;

        float c = cos(a_rotation);
        float s = sin(a_rotation);
        vec2 rotated = vec2(local.x * c - local.y * s,
                            local.x * s + local.y * c) + a_position;

        vec3 world = vec3(rotated, 1.0);
        vec3 ndc   = (a_mode == 0) ? u_screen_matrix * world : u_view_matrix * world;
        gl_Position = vec4(ndc.xy, 0.0, 1.0);

        float u = a_flip.x > 0.0
            ? mix(a_texture_rect.x,                      a_texture_rect.x + a_texture_rect.z, a_pos.x)
            : mix(a_texture_rect.x + a_texture_rect.z,   a_texture_rect.x,                    a_pos.x);
        float v = a_flip.y > 0.0
            ? mix(a_texture_rect.y,                      a_texture_rect.y + a_texture_rect.w, a_pos.y)
            : mix(a_texture_rect.y + a_texture_rect.w,   a_texture_rect.y,                    a_pos.y);

        tex_coord = vec2(u / a_texture_size.x, v / a_texture_size.y);
    }
    // Text
    else if (a_type == 3)
    {
        vec2 local = a_pos * a_size + a_position;
        frag_pos   = vec2(0.0);
        size       = a_size;

        vec3 world = vec3(local, 1.0);
        vec3 ndc   = (a_mode == 0) ? u_screen_matrix * world : u_view_matrix * world;
        gl_Position = vec4(ndc.xy, 0.0, 1.0);

        float u = mix(a_texture_rect.x, a_texture_rect.x + a_texture_rect.z, a_pos.x) / a_texture_size.x;
        float v = mix(a_texture_rect.y, a_texture_rect.y + a_texture_rect.w, a_pos.y) / a_texture_size.y;
        tex_coord = vec2(u, v);
    }
}


#type fragment
#version 300 es
precision highp float;
precision highp int;

// MRT: color + entity id
// NOTE: layout(location) on fragment outputs requires ES 3.0 + GL_EXT_blend_func_extended
// on some drivers. If unavailable, bind locations via glBindFragDataLocation before linking.
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int  entity_id;

in vec4  fill_color;
in vec4  border_color;
in vec2  frag_pos;
in vec2  size;
in float border_size;
in float corner_radius;
in vec2  center;
in vec2  tex_coord;

flat in int v_type;
flat in int v_slot;
flat in int v_entity_id;

// OpenGL ES does not allow dynamic indexing of sampler arrays with a non-constant
// expression in all implementations.  Use two separate uniforms and branch instead.
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

vec4 sample_texture(int slot, vec2 uv) {
    if (slot == 0) return texture(u_texture0, uv);
    if (slot == 1) return texture(u_texture1, uv);
    return texture(u_texture2, uv);
}

void main()
{
    if (v_type == 0)
    {
        // Rectangle
        float total_radius = corner_radius + border_size;
        bool is_corner = corner_radius > 0.0 &&
                         ((frag_pos.x < total_radius || frag_pos.x > size.x - total_radius) &&
                          (frag_pos.y < total_radius || frag_pos.y > size.y - total_radius));

        if (is_corner)
        {
            vec2 corner_center = vec2(
                frag_pos.x < total_radius ? total_radius : size.x - total_radius,
                frag_pos.y < total_radius ? total_radius : size.y - total_radius);
            float dist = distance(frag_pos, corner_center);
            if (dist > total_radius)
                discard;
            else if (dist > corner_radius)
                FragColor = border_color;
            else
                FragColor = fill_color;
        }
        else
        {
            bool is_border = frag_pos.x < border_size || frag_pos.x > size.x - border_size ||
                             frag_pos.y < border_size || frag_pos.y > size.y - border_size;
            FragColor = is_border ? border_color : fill_color;
        }
        entity_id = v_entity_id;
    }
    else if (v_type == 1)
    {
        // Circle / Ellipse
        float radius_x = size.x * 0.5;
        float radius_y = size.y * 0.5;
        vec2  delta    = frag_pos - center;

        float ellipse_dist = (delta.x * delta.x) / (radius_x * radius_x)
                           + (delta.y * delta.y) / (radius_y * radius_y);
        if (ellipse_dist > 1.0)
            discard;

        float inner_rx     = max(radius_x - border_size, 0.001);
        float inner_ry     = max(radius_y - border_size, 0.001);
        float inner_ellipse = (delta.x * delta.x) / (inner_rx * inner_rx)
                            + (delta.y * delta.y) / (inner_ry * inner_ry);
        FragColor = inner_ellipse > 1.0 ? border_color : fill_color;
        entity_id = v_entity_id;
    }
    else if (v_type == 2)
    {
        // Sprite
        vec4 color = sample_texture(v_slot, tex_coord);
        color.a   *= fill_color.a;
        FragColor  = color;
        entity_id  = v_entity_id;
    }
    else if (v_type == 3)
    {
        // Text — MSDF / SDF
        vec3 col   = sample_texture(v_slot, tex_coord).rgb;
        float sd    = median(col.r, col.g, col.b);
        float alpha = smoothstep(0.5 - 0.01, 0.5 + 0.01, sd);
        FragColor   = vec4(fill_color.rgb, fill_color.a * alpha);
        entity_id   = v_entity_id;
    }
}
