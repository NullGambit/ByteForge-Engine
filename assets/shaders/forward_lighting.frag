#version 460 core
out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D texture_sample;
uniform vec4 material_color;

void main()
{
    FragColor = texture(texture_sample, tex_coords) * material_color;
}