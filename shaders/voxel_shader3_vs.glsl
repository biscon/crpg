#version 330 core
layout (location = 0) in vec4 vertexPos;
layout (location = 1) in vec4 vertexColor;
//layout (location = 2) in vec2 texCoord;

out vec3 VertexNormal;
out vec3 VertexPos;
out vec3 VertexColor;
out vec2 TexCoord;

out float AoFactor;
flat out uint FaceIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 normals[6];
uniform vec2 texcoords[4];

void main()
{
    vec3 n = normals[int(vertexPos.w)];

    // calculate and pass vertex pos in world space
    VertexPos = vec3(model * vec4(vertexPos.xyz, 1.0));
    //VertexNormal = mat3(transpose(inverse(model))) * n.xyz;
    //VertexNormal = view * model * vec4(n, 1.0);
    VertexNormal = n;
    TexCoord = texcoords[int(vertexColor.w)];
    VertexColor = vertexColor.xyz;

    // remember to add the stupid 0.5
    uint info = uint(VertexColor.z + 0.5);
    uint i_ao = info & 3u;
    FaceIndex = (info & 0x38u) >> 3;

    float ao = float(i_ao) / 3.0;
    AoFactor = 0.65 + ao * (1.0 - 0.65) / 1.0;

    gl_Position = projection * view * vec4(VertexPos, 1.0);

}
