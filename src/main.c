
#include <stdio.h>
#include "defs.h"
#include "rpg/ability_score.h"
#include "rpg/rpg.h"
#include <memory.h>

#include <SDL.h>
#include <glad.h>

global_variable u32 ScreenWidth = 1280;
global_variable u32 ScreenHeight = 720;
global_variable SDL_Window* Window = NULL;
global_variable SDL_GLContext Context = NULL;
//global_variable SDL_Rect Viewport = {};
global_variable bool ShouldQuit = false;
global_variable u64 InitTimeStamp = 0;
global_variable u64 PerformanceFrequency = 0;

#include <SDL_video.h>
#include <assert.h>


double platformGetTime() {
    return ((double) (SDL_GetPerformanceCounter() - InitTimeStamp)) / (double) PerformanceFrequency;
}

// Internal functions ------------------------------------------------------------------------------

internal void handleKeyboardEvent(const SDL_Event *event)
{
    switch( event->type ) {
        case SDL_KEYDOWN: {
            if(event->key.repeat == 0) {
                // call listeners
                //kb.keysym = event->key.keysym;
            }
            break;
        }
        case SDL_KEYUP: {
            break;
        }
    }
}


internal void handleMouseEvent(const SDL_Event *event)
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

internal void handleWindowEvent(const SDL_Event *event)
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

internal void updateInput() {
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
        handleKeyboardEvent(&e);
        if(e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
        {
            handleMouseEvent(&e);
        }
        if(e.type == SDL_WINDOWEVENT)
        {
            //SDL_Log("Window event");
            handleWindowEvent(&e);
        }
    }
}

internal bool initVideo()
{
    // request a GL Context 3.3 core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // stencil size 8, so far we don't use it
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // request 4x MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

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
    SDL_GL_SetSwapInterval(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

internal void shutdownVideo()
{
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

    if(!initVideo())
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return -1;
    }

    // get info about the current display mode
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        SDL_Log("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
        shutdownVideo();
        SDL_Quit();
        return -1;
    }

    SDL_Log("Monitor refresh rate is %d hz", displayMode.refresh_rate);


    i32 monitorRefreshHz = displayMode.refresh_rate;
    i32 gameUpdateHz = monitorRefreshHz;
    double targetSecondsPerFrame = 1.0 / (double) gameUpdateHz;

    PerformanceFrequency = SDL_GetPerformanceFrequency();
    InitTimeStamp = SDL_GetPerformanceCounter();

    double oldTime = 0;

    //i32 roll = RollDice("2D6+1");
    //SDL_Log("Roll result = %d", roll);
    RPG_Init();

    // game init
    RPGContext rpgContext;
    RPG_InitContext(&rpgContext);

    /*
    AbilityScore score = {0};
    AbilityScore_Roll4D6DropLowest(&score);
    char buffer[100];
    AbilityScore_Print(&score, buffer, sizeof(buffer));
    SDL_Log("AbilityScores:\n%s", buffer);
     */

    Entity entity;
    EntityClass *entityClass = RPG_GetEntityClass(&rpgContext, "Fighter");
    assert(entityClass != NULL);
    Entity_Init(&entity, ET_CHARACTER, 1, "Theodor", entityClass);

    Entity *monster = RPG_CreateMonsterFromTemplate(&rpgContext, RPG_GetMonsterTemplate(&rpgContext, "Swearwolf"), 1);


    while(!ShouldQuit)
    {
        oldTime = platformGetTime();
        updateInput();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            SDL_Log("Loop OpenGL error: %d", err);
        }

        double secondsElapsedForWork = platformGetTime() - oldTime;
        double secondsElapsedForFrame = secondsElapsedForWork;
        while(secondsElapsedForFrame < targetSecondsPerFrame)
        {
            u32 sleepMS = (u32) (1000.0 * (targetSecondsPerFrame - secondsElapsedForFrame));
            if(sleepMS > 10) {
                //SDL_Log("sleepMS %d", sleepMS);
                SDL_Delay(sleepMS);
            }
            secondsElapsedForFrame = platformGetTime() - oldTime;
        }

        //SDL_Log("secondsElapsedForWork %.2f secondsElapsedForFrame %.2f FPS %.2f", secondsElapsedForWork, secondsElapsedForFrame, 1.0/secondsElapsedForFrame);
        // render debug info
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        SDL_GL_SwapWindow(Window);
    }
    RPG_DestroyEntity(monster);

    RPG_ShutdownContext(&rpgContext);

    shutdownVideo();
    SDL_Quit();
    return 0;
}