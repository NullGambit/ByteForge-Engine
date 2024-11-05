#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

out vec3 normal;
out vec2 tex_coords;
out vec3 frag_position;

uniform mat4 pvm;
uniform mat4 model;

void main()
{
    vec4 position = vec4(a_pos, 1.0);

    gl_Position = pvm * position;

    tex_coords      = a_tex_coords;
    normal          = a_normal;
    frag_position   = vec3(model * position);
}
