#version 460 core
out vec4 FragColor;

in vec3 normal;
in vec2 tex_coords;
in vec3 frag_position;

struct Texture
{
    sampler2D texture;
    bool enabled;
    float strength;
    float scale;
};

struct Material
{
    Texture diffuse;
    Texture specular;
    Texture emissive;
};

uniform Material material;

uniform vec3 light_position;
uniform vec3 light_color;

uniform vec3 view_position;

vec4 get_texture(Texture t, vec4 default_value)
{
    if (t.enabled)
    {
        return texture(t.texture, tex_coords * t.scale);
    }
    else
    {
        return default_value;
    }
}

void main()
{
    vec4 object_color = get_texture(material.diffuse, vec4(1.0));

    vec3 ambient = 0.1 * object_color.rgb * light_color;

    vec3 light_direction = normalize(light_position - frag_position);

    float diffuse_factor = max(dot(normalize(normal), light_direction), 0.0);
    vec3 diffuse = (diffuse_factor * object_color.rgb) * light_color;

    vec3 view_direction = normalize(view_position - frag_position);
    vec3 reflection_direction = reflect(-light_direction, normal);

    float specular_factor = pow(max(dot(view_direction, reflection_direction), 0.0), material.specular.strength);
    vec3 specular = (specular_factor * get_texture(material.specular, vec4(1.0)).rgb) * light_color;

    vec3 emissive = get_texture(material.emissive, vec4(0.0)).rgb * material.emissive.strength;

    vec3 result = ambient + diffuse + specular + emissive;

    FragColor = vec4(result.rgb, object_color.a);
}
