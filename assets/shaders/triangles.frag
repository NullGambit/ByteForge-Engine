#version 460 core
out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D texture_sample;

void main()
{
    FragColor = texture(texture_sample, tex_coords);
}