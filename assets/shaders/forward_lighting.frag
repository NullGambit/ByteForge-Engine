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

const int LIGHT_DIRECTION = 0;
const int LIGHT_SPOT = 1;
const int LIGHT_POINT = 2;

struct Light
{
    vec3  position;
    vec3  direction;
    vec3  color;

    float cutoff;
    float outer_cutoff;

    float linear;
    float quadratic;
    bool enabled;

    int type;
};

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

#define MAX_LIGHTS 8

uniform Material material;
uniform Light[MAX_LIGHTS] lights;

uniform vec3 view_position;

struct LightingResult
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
};

vec4 g_object_color;

LightingResult calculate_lighting(vec3 light_direction, vec3 color)
{
    vec3 ambient = color * 0.1 * g_object_color.rgb;

    float diffuse_factor = max(dot(normalize(normal), light_direction), 0.0);
    vec3 diffuse = color * diffuse_factor * g_object_color.rgb;

    vec3 view_direction = normalize(view_position - frag_position);
    vec3 reflection_direction = reflect(-light_direction, normal);

    float specular_factor = pow(max(dot(view_direction, reflection_direction), 0.0), material.specular.strength);
    vec3 specular = color * specular_factor * get_texture(material.specular, vec4(0)).rgb;

    vec3 emissive = get_texture(material.emissive, vec4(0.0)).rgb * material.emissive.strength;

    return LightingResult(ambient, diffuse, specular, emissive);
}

vec3 sum_light(LightingResult result)
{
    return result.ambient + result.diffuse + result.specular + result.emissive;
}

float attenuate(float distance, float linear, float quadratic)
{
    return 1.0 / (1.0 + linear * distance + quadratic * (distance * distance));
}

vec3 calculate_dir_light(Light light)
{
    vec3 light_direction = normalize(-light.direction);

    LightingResult result = calculate_lighting(light_direction, light.color);

    return sum_light(result);
}

//vec3 calculate_point_light(Light light)
//{
//    vec3 light_direction = normalize(light.position - FragPos);
//
//    LightingResult result = calculate_lighting(light_direction);
//
//    float d = length(light.position - FragPos);
//    float attenuation = attenuate(d, light.linear, light.quadratic);
//
//    result.ambient *= attenuation;
//    result.diffuse *= attenuation;
//    result.specular *= attenuation;
//
//    return sum_light(result);
//}

vec3 calculate_point_spot_light(Light light)
{
    vec3 light_direction = normalize(light.position - frag_position);

    LightingResult result = calculate_lighting(light_direction, light.color);

    float intensity     = 1;
    float d             = length(light.position - frag_position);
    float attenuation   = attenuate(d, light.linear, light.quadratic);

    if (light.type == LIGHT_SPOT)
    {
        float theta   = dot(light_direction, normalize(-light.direction));
        float epsilon = light.cutoff - light.outer_cutoff;

        intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
    }

    result.diffuse  *= intensity * attenuation;
    result.specular *= intensity * attenuation;
    result.specular *= intensity * attenuation;

    return sum_light(result);
}

void main()
{
    g_object_color = get_texture(material.diffuse, vec4(1.0));

    vec3 result;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        Light light = lights[i];

        if (light.enabled)
        {
            if (light.type == LIGHT_DIRECTION)
            {
                result += calculate_dir_light(light);
            }
            else
            {
                result += calculate_point_spot_light(light);
            }
        }
    }

    FragColor = vec4(result.rgb, g_object_color.a);
}
