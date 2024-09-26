#version 460 core
out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    FragColor = mix(texture(texture0, tex_coords), texture(texture1, tex_coords), 0.3);
}