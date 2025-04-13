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

    float intensity;
    float cutoff;
    float outer_cutoff;
    float max_distance;

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

#define MAX_LIGHTS 16

uniform Material material;
uniform Light[MAX_LIGHTS] lights;

uniform vec3 view_position;

struct LightingResult
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float intensity;
};

vec4 g_object_color;

LightingResult calculate_lighting(Light light, vec3 light_direction)
{
    vec3 ambient = light.color * 0.1 * g_object_color.rgb;

    float diffuse_factor = max(dot(normalize(normal), light_direction), 0.0);
    vec3 diffuse = light.color * diffuse_factor * g_object_color.rgb;

    vec3 view_direction = normalize(view_position - frag_position);
    vec3 reflection_direction = reflect(-light_direction, normal);

    float specular_factor = pow(max(dot(view_direction, reflection_direction), 0.0), material.specular.strength);
    vec3 specular = light.color * specular_factor * get_texture(material.specular, vec4(0)).rgb;

    vec3 emissive = get_texture(material.emissive, vec4(0.0)).rgb * material.emissive.strength;

    return LightingResult(ambient, diffuse, specular, emissive, light.intensity);
}

vec3 sum_light(LightingResult result)
{
    return result.ambient + result.diffuse + result.specular + result.emissive + (result.intensity * 0.001);
}

float attenuate(float distance, float max_distance)
{
    float Kl = 2.0 / max_distance;
    float Kq = 1.0 / (max_distance * max_distance);

    return 1.0 / (1.0 + Kl * distance + Kq * (distance * distance));
}

vec3 calculate_dir_light(Light light)
{
    vec3 light_direction = normalize(-light.direction);

    LightingResult result = calculate_lighting(light, light_direction);

    return sum_light(result);
}

vec3 calculate_point_spot_light(Light light)
{
    vec3 light_direction = normalize(light.position - frag_position);

    LightingResult result = calculate_lighting(light, light_direction);

    light.max_distance = max(light.max_distance, 1.0);

    float intensity     = 1;
    float d             = length(light.position - frag_position);
    float attenuation   = attenuate(d, light.max_distance);

    if (light.type == LIGHT_SPOT)
    {
        float theta   = dot(light_direction, normalize(-light.direction));
        float epsilon = light.cutoff - light.outer_cutoff;

        intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
    }

    result.diffuse  *= intensity * attenuation;
    result.ambient  *= intensity * attenuation;
    result.specular *= intensity * attenuation;

    return sum_light(result);
}

void main()
{
    g_object_color = get_texture(material.diffuse, vec4(1.0));

    vec3 result = vec3(0.0);

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
