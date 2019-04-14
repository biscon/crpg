
#include <stdio.h>
#include "defs.h"
#include "rpg/ability_score.h"
#include "rpg/rpg.h"
#include "rpg/combat.h"
#include "rpg/rpg_log.h"
#include "renderer/opengl_renderer.h"
#include "renderer/terminal.h"
#include "util/rex.h"
#include "input/input.h"
#include "game/game.h"
#include "game/game_state.h"
#include "game/combat_state.h"
#include "game/intro_state.h"
#include <memory.h>

#include <SDL.h>
#include <glad.h>

global_variable u32 ScreenWidth = 1920;
global_variable u32 ScreenHeight = 1080;
global_variable SDL_Window* Window = NULL;
global_variable SDL_GLContext Context = NULL;
//global_variable SDL_Rect Viewport = {};
global_variable bool ShouldQuit = false;
global_variable u64 InitTimeStamp = 0;
global_variable u64 PerformanceFrequency = 0;

#include <SDL_video.h>
#include <assert.h>


double GetTime() {
    return ((double) (SDL_GetPerformanceCounter() - InitTimeStamp)) / (double) PerformanceFrequency;
}

// Internal functions ------------------------------------------------------------------------------

INTERNAL void HandleKeyboardEvent(const SDL_Event *event)
{
    switch( event->type ) {
        case SDL_KEYDOWN: {
            if(event->key.repeat == 0) {
                Input_PushKeyDown(event->key.keysym.sym);
            }
            break;
        }
        case SDL_KEYUP: {
            Input_PushKeyUp(event->key.keysym.sym);
            break;
        }
    }
}


INTERNAL void HandleMouseEvent(const SDL_Event *event)
{
    switch( event->type ) {
        case SDL_MOUSEMOTION: {

            break;
        }
        case SDL_MOUSEWHEEL: {
            break;
        }
    }
}

INTERNAL void HandleWindowEvent(const SDL_Event *event)
{
    switch( event->window.event ) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            // call listeners

            break;
        }
        case SDL_WINDOWEVENT_ENTER: {

            break;
        }
        case SDL_WINDOWEVENT_LEAVE: {

            break;
        }
        case SDL_WINDOWEVENT_FOCUS_GAINED: {

            break;
        }
        case SDL_WINDOWEVENT_FOCUS_LOST: {

            break;
        }
    }
}

INTERNAL void UpdateInput() {
    //Handle events on queue
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            ShouldQuit = true;
            return;
        }
        HandleKeyboardEvent(&e);
        if(e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
        {
            HandleMouseEvent(&e);
        }
        if(e.type == SDL_WINDOWEVENT)
        {
            //SDL_Log("Window event");
            HandleWindowEvent(&e);
        }
    }
}

INTERNAL bool InitVideo()
{
    // request a GL Context 3.3 core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // stencil size 8, so far we don't use it
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // request 4x MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    Window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    if(Window == NULL)
    {
        SDL_Log("Could not create Window\n");
        return false;
    }
    Context = SDL_GL_CreateContext(Window);
    if(Context == NULL)
    {
        SDL_Log("Could not create Context\n");
        return false;
    }

    // Load OpenGL functions glad SDL
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_Log("Vendor:          %s\n", glGetString(GL_VENDOR));
    SDL_Log("Renderer:        %s\n", glGetString(GL_RENDERER));
    SDL_Log("Version OpenGL:  %s\n", glGetString(GL_VERSION));
    SDL_Log("Version GLSL:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // request vsync
    //SDL_GL_SetSwapInterval(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, ScreenWidth, ScreenHeight);
    OGL_InitRenderer(ScreenWidth, ScreenHeight);
    return true;
}

INTERNAL void ShutdownVideo()
{
    OGL_ShutdownRenderer();
    if(Context != NULL)
        SDL_GL_DeleteContext(Context);
    if(Window != NULL)
        SDL_DestroyWindow(Window);
}

/**
 * Entry point
 * @return
 */

int main()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return -1;
    }

    if(!InitVideo())
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return -1;
    }

    RenderCmdBuffer renderBuffer;
    Render_CreateCmdBuffer(&renderBuffer);

    // get info about the current display mode
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        SDL_Log("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
        ShutdownVideo();
        SDL_Quit();
        return -1;
    }
    SDL_Log("Monitor refresh rate is %d hz", displayMode.refresh_rate);

    i32 monitorRefreshHz = displayMode.refresh_rate;
    i32 gameUpdateHz = monitorRefreshHz;
    //i32 gameUpdateHz = 620;
    double targetSecondsPerFrame = 1.0 / (double) gameUpdateHz;
    PerformanceFrequency = SDL_GetPerformanceFrequency();
    InitTimeStamp = SDL_GetPerformanceCounter();
    double oldTime = 0;

    Input_Init();
    RPG_Init();
    Game_Init();

    while(!ShouldQuit)
    {
        oldTime = GetTime();
        UpdateInput();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            SDL_Log("Loop OpenGL error: %d", err);
        }

        double secondsElapsedForWork = GetTime() - oldTime;
        double secondsElapsedForFrame = secondsElapsedForWork;
        while(secondsElapsedForFrame < targetSecondsPerFrame)
        {
            u32 sleepMS = (u32) (1000.0 * (targetSecondsPerFrame - secondsElapsedForFrame));
            if(sleepMS > 10) {
                //SDL_Log("sleepMS %d", sleepMS);
                SDL_Delay(sleepMS);
            }
            secondsElapsedForFrame = GetTime() - oldTime;
        }

        Render_ClearCmdBuffer(&renderBuffer);
        Render_PushClearCmd(&renderBuffer, (vec4) {0, 0, 0, 1.0f});
        Game_Update(&renderBuffer, secondsElapsedForFrame);

        OGL_RenderCmdBuffer(&renderBuffer);
        SDL_GL_SwapWindow(Window);
    }

    Game_Shutdown();
    Render_DestroyCmdBuffer(&renderBuffer);
    Input_Shutdown();
    ShutdownVideo();
    SDL_Quit();
    return 0;
}