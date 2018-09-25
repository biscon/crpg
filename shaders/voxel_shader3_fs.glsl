#version 330 core
out vec4 FragColor;

uniform int layers[6];
uniform vec4 colors[6];
uniform sampler2DArray array_texture;

struct Fog
{
    int is_active;
    vec3 color;
    float density;
};

in vec3 VertexPos;
in vec3 VertexNormal;
in vec3 VertexColor;
in vec2 TexCoord;
in float AoFactor;
flat in uint FaceIndex;

uniform vec3 camPos;
uniform Fog fog;
uniform float sunlightIntensity;

vec4 calcFog(vec3 pos, vec4 color, Fog fog)
{
    vec3 fogColor = fog.color;

    float distance = length(pos);
    float fogFactor = 1.0 / exp( (distance * fog.density)* (distance * fog.density));
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    vec3 resultColor = mix(fogColor, color.xyz, fogFactor);
    return vec4(resultColor.xyz, color.w);
}

void main()
{
    // properties
    //vec3 norm = normalize(VertexNormal);
    vec4 result;
    //result = texture(texture_diffuse, TexCoord);
    float sun = sunlightIntensity * (VertexColor.x / 15.0);
    float torch = VertexColor.y / 15.0;

    vec3 torch_color = vec3(1.0, 0.95, 0.90);

    vec3 sunlight = vec3(sun, sun, sun);
    vec3 torchlight = torch * torch_color;
    vec3 light = clamp(sunlight + torchlight, 0.05, 1.0);

    vec3 final_color = mix(vec3(0,0,0), light, AoFactor);

    if(layers[FaceIndex] != -1)
    {
        vec3 array_tex_coord = vec3(TexCoord, layers[FaceIndex]);
        result = vec4(final_color, 1.0) * texture(array_texture, array_tex_coord);
    }
    else
    {
        result = vec4(final_color, 1.0) * colors[FaceIndex];
    }

    FragColor = result;

    if(fog.is_active == 1)
    {
        FragColor = calcFog(camPos - VertexPos, FragColor, fog);
    }

    // apply gamma correction
    //float gamma = 2.2;
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}