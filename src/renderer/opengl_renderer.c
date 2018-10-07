//
// Created by bison on 10/7/18.
//

#include <SDL_log.h>
#include "opengl_renderer.h"
#include <glad.h>

internal i32 quadVBO;
internal i32 textQuadVBO;
internal GLuint shaderProgramID;

internal const char* vertexSource2D = R"glsl(
    #version 330 core
    // read from vertex buffer
    in vec2 position;
    in vec4 color;
    in vec2 texcoord;
    // pass to fragment shader
    out vec4 Color;
    out vec2 Texcoord;
    // uniforms
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    void main()
    {
        Color = color;
        Texcoord = texcoord;
        gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
    }
)glsl";

internal const char* fragmentSource2D = R"glsl(
    #version 330 core
    // passed from vertex shader
    in vec4 Color;
    in vec2 Texcoord;
    // resulting fragment color
    out vec4 outColor;
    // texture
    uniform sampler2D tex;
    void main()
    {
        outColor = texture(tex, Texcoord) * Color;
        //outColor = vec4(Color, 1.0);
    }
)glsl";

internal void CheckCompileErrors(GLuint shader, const char* type)
{
    GLint success;
    GLchar infoLog[1024];
    if(strcmp("PROGRAM", type) != 0)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            SDL_Log("Shader compilation error: %s\n%s", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            SDL_Log("Shader program link error: %s\n%s", type, infoLog);
        }
    }
}


void Render_InitOGLRenderer() {
    u32 vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource2D, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource2D, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");

    // shader Program
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertex);
    glAttachShader(shaderProgramID, fragment);
    glLinkProgram(shaderProgramID);
    CheckCompileErrors(shaderProgramID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Render_ShutdownOGLRenderer() {
    glDeleteProgram(shaderProgramID);
}

void Render_RenderCmdBufferOGL(RenderCmdBuffer *buf)
{
    u8 *cur_ptr = buf->commands;
    while(cur_ptr < buf->cmdOffset) {
        RenderCmd *command = (RenderCmd*) cur_ptr;
        switch(command->type)
        {
            case RCMD_CLEAR: {
                RenderCmdClear *cmd = (RenderCmdClear*) cur_ptr;
                SDL_Log("Rendering clear command\n;");
                cur_ptr += sizeof(RenderCmdClear);
                break;
            }
            case RCMD_QUAD: {
                RenderCmdQuads *cmd = (RenderCmdQuads*) cur_ptr;
                SDL_Log("Rendering quad command\n;");
                cur_ptr += sizeof(RenderCmdQuads);
                break;
            }
            default: {
                break;
            }
        }
    }
}

