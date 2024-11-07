#version 460 core
out vec4 FragColor;

in vec3 normal;
in vec2 tex_coords;
in vec3 frag_position;

uniform sampler2D texture_sample;
uniform vec4 material_color;
uniform float specular_strength;
uniform bool enable_diffuse;

uniform vec3 light_position;
uniform vec3 light_color;

uniform vec3 view_position;

void main()
{
    float ambient_factor = 0.1;
    vec3 ambient = ambient_factor * light_color;

    vec3 light_direction = normalize(light_position - frag_position);

    float diffuse_factor = max(dot(normalize(normal), light_direction), 0.0);
    vec3 diffuse = diffuse_factor * light_color;

    vec3 view_direction = normalize(view_position - frag_position);
    vec3 reflection_direction = reflect(-light_direction, normal);

    float specular_factor = pow(max(dot(view_direction, reflection_direction), 0.0), 32);
    vec3 specular = specular_factor * specular_strength * light_color;

    vec4 object_color = material_color;

    if (enable_diffuse)
    {
        object_color *= texture(texture_sample, tex_coords);
    }

    vec3 result = (ambient + diffuse + specular)  * object_color.rgb;

    FragColor = vec4(result, object_color.a);
}