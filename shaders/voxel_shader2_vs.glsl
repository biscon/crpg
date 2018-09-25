#version 330 core
layout (location = 0) in vec4 vertexPos;
layout (location = 1) in vec4 vertexColor;
//layout (location = 2) in vec2 texCoord;

out vec3 VertexNormal;
out vec3 VertexPos;
out vec2 TexCoord;

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
    VertexNormal = mat3(transpose(inverse(model))) * n.xyz;
    TexCoord = texcoords[int(vertexColor.w)];
    gl_Position = projection * view * vec4(VertexPos, 1.0);

}
