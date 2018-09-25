#version 330 core
out vec4 FragColor;

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    int hasTexture;
    float shininess;
};

struct DirLight {
    vec3 color;
    vec3 direction;
    float intensity;
    int is_active;
};

struct Fog
{
    int is_active;
    vec3 color;
    float density;
};

struct Attenuation
{
    float constant;
    float linear;
    float exponent;
};

struct PointLight
{
    vec3 color;
    // Light position is assumed to be in view coordinates
    vec3 position;
    float intensity;
    Attenuation att;
    int is_active;
};

in vec3 VertexPos;
in vec3 VertexNormal;
in vec2 TexCoord;

uniform vec3 camPos;
uniform vec3 ambientLight;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform Material material;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform Fog fog;

// globals used for lighting calculations
vec4 ambientColor;
vec4 diffuseColor;
vec4 specularColor;

vec4 calcFog(vec3 pos, vec4 color, Fog fog, vec3 ambientLight, DirLight dirLight)
{
    vec3 fogColor = fog.color * (ambientLight + dirLight.color * dirLight.intensity);

    float distance = length(pos);
    float fogFactor = 1.0 / exp( (distance * fog.density)* (distance * fog.density));
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    vec3 resultColor = mix(fogColor, color.xyz, fogFactor);
    return vec4(resultColor.xyz, color.w);
}

void setupColors(Material material, vec2 textCoord)
{
    if (material.hasTexture == 1)
    {
        ambientColor = texture(texture_diffuse, textCoord);
        diffuseColor = ambientColor;
        specularColor = texture(texture_specular, textCoord);
    }
    else
    {
        ambientColor = material.ambient;
        diffuseColor = material.diffuse;
        specularColor = material.specular;
    }
}

// calculates the color when using a directional light.
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = diffuseColor * vec4(light.color, 1.0) * light.intensity * diff;

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = specularColor * light.intensity * spec * vec4(light.color, 1.0);
    // combine results

    return (diffuse + specular);
}


void main()
{
    setupColors(material, TexCoord);
    // properties
    vec3 norm = normalize(VertexNormal);
    vec3 viewDir = normalize(camPos - VertexPos);

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    float intensity = dirLight.intensity;
    if(intensity < 0.05)
        intensity = 0.05;
    vec4 result = vec4(intensity * ambientLight, 1.0) * ambientColor;

    result += CalcDirLight(dirLight, norm, viewDir);

    FragColor = result;

    if(fog.is_active == 1)
    {
        FragColor = calcFog(camPos - VertexPos, FragColor, fog, ambientLight, dirLight);
    }

    // apply gamma correction
    //float gamma = 2.2;
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}