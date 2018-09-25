#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;

out vec3 VertexNormal;
out vec3 VertexPos;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // calculate and pass vertex pos in world space
    VertexPos = vec3(model * vec4(vertexPos, 1.0));
    VertexNormal = mat3(transpose(inverse(model))) * vertexNormal;
    //VertexNormal = normalize(model * view * vec4(vertexNormal, 0.0)).xyz;
    TexCoord = texCoord;
    gl_Position = projection * view * vec4(VertexPos, 1.0);

}
