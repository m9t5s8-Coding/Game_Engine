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
    border_size   = abs(a_border_thickness);
    v_type        = a_type;
    v_slot        = a_slot;
    v_entity_id   = a_entity_id;

    // Rectangle or Circle.
    //
    // A positive border expands the quad outwards. A negative border is drawn
    // inside the original quad (the editor uses this for collision overlays).
    // Hand the fragment shader the outer extents and the outer corner radius;
    // it derives the inner fill region by subtracting the border width. This
    // keeps both border directions consistent and gives the SDF branch enough
    // information to antialias the outer and inner edges independently.
    if (a_type == 0 || a_type == 1)
    {
        float expand     = max(a_border_thickness, 0.0);
        vec2  total_size = max(a_size + vec2(expand * 2.0), vec2(0.0));

        size          = total_size;
        center        = total_size * 0.5;
        frag_pos      = a_pos * total_size;
        corner_radius = max(a_corner_radius + expand, 0.0);

        vec2 local = a_pos * total_size - center;
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

        // Map the quad to texel centres inside the source rect. Sampling the
        // geometric rect edges directly makes linear filtering read the
        // neighbouring atlas cell or tile.
        vec2 tex_size = max(a_texture_size, vec2(1.0));
        vec2 uv_a     = (a_texture_rect.xy + 0.5) / tex_size;
        vec2 uv_b     = (a_texture_rect.xy + a_texture_rect.zw - 0.5) / tex_size;
        vec2 uv_min   = min(uv_a, uv_b);
        vec2 uv_max   = max(uv_a, uv_b);

        float u = a_flip.x > 0.0 ? mix(uv_min.x, uv_max.x, a_pos.x)
                                 : mix(uv_max.x, uv_min.x, a_pos.x);
        float v = a_flip.y > 0.0 ? mix(uv_min.y, uv_max.y, a_pos.y)
                                 : mix(uv_max.y, uv_min.y, a_pos.y);

        tex_coord = vec2(u, v);
    }
    // Text
    else if (a_type == 3)
    {
        // Combined text path: a_position is the glyph's top-left corner and
        // a_origin is the text pivot supplied by the CPU. Rotating around the
        // shared pivot keeps the whole line together instead of rotating each
        // glyph around its own centre.
        vec2 local = a_pos * a_size + a_position;
        vec2 rel   = local - a_origin;

        float c = cos(a_rotation);
        float s = sin(a_rotation);
        vec2 rotated = vec2(rel.x * c - rel.y * s,
                            rel.x * s + rel.y * c) + a_origin;

        frag_pos = vec2(0.0);
        size     = a_size;

        vec3 world = vec3(rotated, 1.0);
        vec3 ndc   = (a_mode == 0) ? u_screen_matrix * world : u_view_matrix * world;
        gl_Position = vec4(ndc.xy, 0.0, 1.0);

        vec2 tex_size = max(a_texture_size, vec2(1.0));
        vec2 uv_a     = (a_texture_rect.xy + 0.5) / tex_size;
        vec2 uv_b     = (a_texture_rect.xy + a_texture_rect.zw - 0.5) / tex_size;
        tex_coord     = mix(min(uv_a, uv_b), max(uv_a, uv_b), a_pos);
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
// expression in all implementations. Use separate uniforms and branch instead.
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

vec4 sample_texture(int slot, vec2 uv)
{
    if (slot == 0) return texture(u_texture0, uv);
    if (slot == 1) return texture(u_texture1, uv);
    return texture(u_texture2, uv);
}

// Signed distance to a rounded box. Negative inside, positive outside.
float sd_rounded_box(vec2 p, vec2 half_size, float radius)
{
    half_size = max(half_size, vec2(0.0));
    radius    = clamp(radius, 0.0, min(half_size.x, half_size.y));
    vec2 q    = abs(p) - half_size + vec2(radius);
    return length(max(q, vec2(0.0))) + min(max(q.x, q.y), 0.0) - radius;
}

// Dimensionless signed distance to an ellipse. fwidth() turns it into a
// screen-space coverage ramp below, so no pixel-space scaling is required.
float sd_ellipse(vec2 p, vec2 radii)
{
    radii = max(radii, vec2(0.0001));
    return length(p / radii) - 1.0;
}

// Combine the outer coverage with the fill coverage. The result is a
// straight-alpha colour ready for GL_SRC_ALPHA / GL_ONE_MINUS_SRC_ALPHA.
vec4 shade_shape(float d_outer, float d_inner, bool has_inner)
{
    float aa_outer  = max(fwidth(d_outer), 0.0001);
    float outer_cov = 1.0 - smoothstep(-aa_outer, aa_outer, d_outer);

    // Without a border only the fill colour exists; mixing in border_color at
    // the antialiased rim would darken the edge and reduce its alpha.
    if (border_size <= 0.0)
    {
        vec4 color = fill_color;
        color.a *= outer_cov;
        return color;
    }

    float aa_inner  = max(fwidth(d_inner), 0.0001);
    float inner_cov = has_inner ? (1.0 - smoothstep(-aa_inner, aa_inner, d_inner)) : 0.0;

    vec4 color = mix(border_color, fill_color, inner_cov);
    color.a *= outer_cov;
    return color;
}

void main()
{
    if (v_type == 0)
    {
        // Rectangle
        vec2  p          = frag_pos - center;
        vec2  outer_half = max(size * 0.5, vec2(0.0));
        vec2  inner_half = outer_half - vec2(border_size);
        float outer_rad  = clamp(corner_radius, 0.0, min(outer_half.x, outer_half.y));
        float inner_rad  = max(outer_rad - border_size, 0.0);

        bool has_inner = inner_half.x > 0.0 && inner_half.y > 0.0;
        inner_half     = max(inner_half, vec2(0.0));

        vec4 color = shade_shape(sd_rounded_box(p, outer_half, outer_rad),
                                 sd_rounded_box(p, inner_half, inner_rad),
                                 has_inner);
        if (color.a <= 0.001)
            discard;

        FragColor = color;
        entity_id = v_entity_id;
    }
    else if (v_type == 1)
    {
        // Circle / ellipse
        vec2 p       = frag_pos - center;
        vec2 outer_r = max(size * 0.5, vec2(0.0));
        vec2 inner_r = outer_r - vec2(border_size);

        bool has_inner = inner_r.x > 0.0 && inner_r.y > 0.0;
        inner_r        = max(inner_r, vec2(0.0));

        vec4 color = shade_shape(sd_ellipse(p, outer_r),
                                 sd_ellipse(p, inner_r),
                                 has_inner);
        if (color.a <= 0.001)
            discard;

        FragColor = color;
        entity_id = v_entity_id;
    }
    else if (v_type == 2)
    {
        // Sprite
        vec4 color = sample_texture(v_slot, tex_coord);
        color     *= fill_color; // RGB tint and alpha
        if (color.a <= 0.001)
            discard;

        FragColor = color;
        entity_id = v_entity_id;
    }
    else if (v_type == 3)
    {
        // Text with MSDF
        vec3 col = sample_texture(v_slot, tex_coord).rgb;
        float sd = median(col.r, col.g, col.b);

        // Screen-space derivative keeps the edge one pixel wide at any font
        // size. This matches the dedicated TextES.glsl path.
        float smoothing = max(fwidth(sd), 0.0001);
        float alpha     = smoothstep(0.5 - smoothing, 0.5 + smoothing, sd);
        if (alpha <= 0.001)
            discard;

        FragColor = vec4(fill_color.rgb, fill_color.a * alpha);
        entity_id = v_entity_id;
    }
}
