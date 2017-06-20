#include "renderer2d.h"

const Graphics::ShaderSource &Renderer2D::DefaultShaderSource()
{
    static Graphics::ShaderSource src
    {
        {"a_pos", "a_color", "a_texcoord", "a_factors"},
        {"u_matrix", "u_texture", "u_texture_size", "u_colormatrix"},
        ForPC("#version 330 compatibility")
        ForMobile("#version 100")
        R"(
uniform mat4 u_matrix;
uniform vec2 u_texture_size;

attribute vec2 a_pos;
attribute vec4 a_color;
attribute vec2 a_texcoord;
attribute vec3 a_factors;

varying vec4 v_color;
varying vec2 v_texcoord;
varying vec3 v_factors;

void main()
{
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    v_color    = a_color;
    v_texcoord = a_texcoord / u_texture_size;
    v_factors  = a_factors;
}
)",
        ForPC("#version 330 compatibility")
        ForMobile("#version 100\nprecision mediump float;")
        R"(
uniform sampler2D u_texture;
uniform mat4 u_colormatrix;

varying vec4 v_color;
varying vec2 v_texcoord;
varying vec3 v_factors;

void main()
{
    vec4 tex_color = texture2D(u_texture, v_texcoord);
    gl_FragColor = vec4(v_color.rgb * (1. - v_factors.x) + tex_color.rgb * v_factors.x, v_color.a * (1. - v_factors.y) + tex_color.a * v_factors.y);
    gl_FragColor.rgb = (u_colormatrix * vec4(gl_FragColor.rgb, 1)).rgb * gl_FragColor.a;
    gl_FragColor.a *= v_factors.z;
}
)"
    };
    return src;
}
