//
// Created by bison on 10/7/18.
//

#include <SDL_log.h>
#include "opengl_renderer.h"
#include "../util/lodepng.h"
#include <glad.h>
#include <cglm/cam.h>

//INTERNAL i32 quadVBO;
INTERNAL GLuint texQuadVBO;
INTERNAL GLuint texQuadVAO;
//INTERNAL GLuint quadProgramID;
INTERNAL GLuint texQuadProgramID;
INTERNAL GLint uniformModelLoc;
INTERNAL GLint uniformViewLoc;
INTERNAL GLint uniformProjLoc;
INTERNAL GLint uniformUseTextureLoc;
INTERNAL GLint uniformTextureLoc;
INTERNAL i32 screenWidth;
INTERNAL i32 screenHeight;

/*
INTERNAL const char* quadVertexSource = R"glsl(
    #version 330 core
    // read from vertex buffer
    layout (location = 0) in vec2 position;
    layout (location = 1) in vec4 color;

    // pass to fragment shader
    out vec4 Color;

    // uniforms
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;

    void main()
    {
        Color = color;
        gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
    }
)glsl";

INTERNAL const char* quadFragmentSource = R"glsl(
    #version 330 core
    // passed from vertex shader
    in vec4 Color;

    // resulting fragment color
    out vec4 outColor;

    void main()
    {
        outColor = Color;
    }
)glsl";
*/

INTERNAL const char* texQuadVertexSource = R"glsl(
    #version 330 core
    // read from vertex buffer
    layout (location = 0) in vec2 position;
    layout (location = 1) in vec4 color;
    layout (location = 2) in vec2 texcoord;

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

INTERNAL const char* texQuadFragmentSource = R"glsl(
    #version 330 core
    // passed from vertex shader
    in vec4 Color;
    in vec2 Texcoord;

    // resulting fragment color
    out vec4 outColor;

    // texture
    uniform sampler2D tex;
    uniform int use_texture;

    void main()
    {
        if(use_texture == 1)
            outColor = texture(tex, Texcoord) * Color;
        else
            outColor = Color;
    }
)glsl";

INTERNAL void CheckCompileErrors(GLuint shader, const char* type)
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

INTERNAL GLuint CreateShaderProgram(const char* vertexsrc, const char* fragmentsrc)
{
    u32 vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexsrc, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentsrc, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");

    // shader Program
    GLuint id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    CheckCompileErrors(id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return id;
}


void OGL_InitRenderer(i32 screenw, i32 screenh) {
    screenWidth = screenw;
    screenHeight = screenh;
    //quadProgramID = CreateShaderProgram(quadVertexSource, quadFragmentSource);
    texQuadProgramID = CreateShaderProgram(texQuadVertexSource, texQuadFragmentSource);

    // resolve uniform locations
    uniformModelLoc = glGetUniformLocation(texQuadProgramID, "model");
    uniformViewLoc = glGetUniformLocation(texQuadProgramID, "view");
    uniformProjLoc = glGetUniformLocation(texQuadProgramID, "proj");
    uniformUseTextureLoc = glGetUniformLocation(texQuadProgramID, "use_texture");
    uniformTextureLoc = glGetUniformLocation(texQuadProgramID, "tex");

    glUseProgram(texQuadProgramID);

    // generate and bind VAO
    glGenVertexArrays(1, &texQuadVAO);
    glBindVertexArray(texQuadVAO);

    // generate and bind buffer
    glGenBuffers(1, &texQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, texQuadVBO);

    GLsizei bytes_vertex = FLOATS_PER_VERTEX * sizeof(float);
    // setup vertex attributes
    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, bytes_vertex, (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, bytes_vertex, (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // tex coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, bytes_vertex, (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    Font_Init();
}

void OGL_ShutdownRenderer() {
    Font_Shutdown();

    glDeleteBuffers(1, &texQuadVBO);
    glDeleteVertexArrays(1, &texQuadVAO);
    glDeleteProgram(texQuadProgramID);
    //glDeleteProgram(quadProgramID);
}

void OGL_RenderCmdBuffer(RenderCmdBuffer *buf)
{
    glUseProgram(texQuadProgramID);

    // bind vao
    glBindVertexArray(texQuadVAO);
    // upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, texQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (buf->quadVertOffset - buf->quadVerts), buf->quadVerts, GL_STREAM_DRAW);

    // TODO remove model and view matrices, not needed for 2d
    // setup matrices, right now we only really need the projection matrix
    mat4 model = {};
    mat4 view = {};
    mat4 proj = {};

    glm_mat4_identity(model);
    glm_mat4_identity(view);
    glViewport(0,0, screenWidth, screenHeight);
    glm_ortho(0, screenWidth, screenHeight, 0, -100.0f, 100.0f, proj);

    // set uniforms
    glUniformMatrix4fv(uniformModelLoc, 1, GL_FALSE, (float*) model);
    glUniformMatrix4fv(uniformViewLoc, 1, GL_FALSE, (float*) view);
    glUniformMatrix4fv(uniformProjLoc, 1, GL_FALSE, (float*) proj);

    // bind sampler to texture unit 0
    glUniform1i(uniformTextureLoc, 0);

    // disable depth testing since we're using blending and painters algorithm
    glDisable(GL_DEPTH_TEST);

    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    u8 *cur_ptr = buf->commands;
    while(cur_ptr < buf->cmdOffset) {
        RenderCmd *command = (RenderCmd*) cur_ptr;
        switch(command->type)
        {
            case RCMD_CLEAR: {
                RenderCmdClear *cmd = (RenderCmdClear*) cur_ptr;
                //SDL_Log("Rendering clear command");
                cur_ptr += sizeof(RenderCmdClear);
                glClearColor(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]);
                glClear(GL_COLOR_BUFFER_BIT);
                break;
            }
            case RCMD_QUAD: {
                RenderCmdQuads *cmd = (RenderCmdQuads*) cur_ptr;
                //SDL_Log("Rendering quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld", cmd->offset, cmd->vertexCount, cmd->vertexOffset);
                cur_ptr += sizeof(RenderCmdQuads);
                glUniform1i(uniformUseTextureLoc, 0);
                glDrawArrays(GL_TRIANGLES, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                break;
            }
            case RCMD_TEX_QUAD: {
                RenderCmdQuads *cmd = (RenderCmdQuads*) cur_ptr;
                //SDL_Log("Rendering quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld", cmd->offset, cmd->vertexCount, cmd->vertexOffset);
                cur_ptr += sizeof(RenderCmdQuads);
                glUniform1i(uniformUseTextureLoc, 1);
                glBindTexture(GL_TEXTURE_2D, cmd->texId);
                glDrawArrays(GL_TRIANGLES, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                break;
            }
            case RCMD_TEX_QUAD_ATLAS: {
                RenderCmdQuads *cmd = (RenderCmdQuads*) cur_ptr;
                //SDL_Log("Rendering quad command offset = %ld, vertexcount = %ld, vertexoffset = %ld", cmd->offset, cmd->vertexCount, cmd->vertexOffset);
                cur_ptr += sizeof(RenderCmdQuads);
                glUniform1i(uniformUseTextureLoc, 1);
                glBindTexture(GL_TEXTURE_2D, cmd->texId);
                glDrawArrays(GL_TRIANGLES, (GLsizei) cmd->vertexOffset, (GLsizei) cmd->vertexCount);
                break;
            }
            default: {
                break;
            }
        }
    }
}

bool OGL_UploadPNGTexture(const char *filename, bool filtering, u32 *texid) {
    PixelBuffer pb;
    if(!PixelBuffer_CreateFromPNG(&pb, filename)) {
        SDL_Log("Error loading png %s", filename);
        return false;
    }
    bool res = OGL_UploadTexture(&pb, filtering, texid);
    PixelBuffer_Destroy(&pb);
    return res;
}

bool OGL_UploadTexture(PixelBuffer *pb, bool filtering, u32 *texid)
{
    u32 tex = 0;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    // upload pixel data to gpu mem
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pb->width, pb->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) pb->pixels);

    // set wrap repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     */
    // nearest neighbour filtering
    if(!filtering) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        // anisotropic filtering
        /*
        float aniso = 16.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
        */
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }


    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("UploadTextureOGL OpenGL error: %d", err);
        return false;
    }

    *texid = tex;
    return true;
}

bool OGL_UploadTextureGreyscale(PixelBuffer *pb, bool filtering, u32 *texid)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    u32 tex = 0;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    // upload pixel data to gpu mem
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pb->width, pb->height, 0, GL_RED, GL_UNSIGNED_BYTE, pb->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // nearest neighbour filtering
    if(!filtering) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("UploadTextureOGL OpenGL error: %d", err);
        return false;
    }

    *texid = tex;
    return true;
}


void OGL_DeleteTexture(u32 *tex) {
    glDeleteTextures(1, (GLuint*) &tex);
}


