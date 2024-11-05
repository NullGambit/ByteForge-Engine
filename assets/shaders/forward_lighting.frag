#version 460 core
out vec4 FragColor;

in vec3 normal;
in vec2 tex_coords;
in vec3 frag_position;

uniform sampler2D texture_sample;
uniform vec4 material_color;

uniform vec3 light_position;
uniform vec3 light_color;

void main()
{
    float ambient_factor = 0.1;
    vec3 ambient = ambient_factor * light_color;

    vec3 light_direction = normalize(light_position - frag_position);

    float diffuse_factor = max(dot(normalize(normal), light_direction), 0.0);
    vec3 diffuse = diffuse_factor * light_color;

    vec4 object_color = texture(texture_sample, tex_coords) * material_color;
    vec3 result = (ambient + diffuse)  * object_color.rgb;

    FragColor = vec4(result, object_color.a);
}