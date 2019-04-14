//
// Created by bison on 4/14/19.
//

#include <SDL_log.h>
#include "intro_state.h"

#include "game_state.h"

INTERNAL TextureAtlas atlas;

INTERNAL AtlasQuad atlasquad1 =  {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .atlasId = -1,
        .left = 0, .top = 0, .right = 160, .bottom = 160};

INTERNAL AtlasQuad atlasquad2 = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .atlasId = -1,
        .left = 160, .top = 160, .right = 320, .bottom = 320};


INTERNAL Quad quad1 = {.color = {1.0f, 1.0f, 1.0f, 1.0f},
                      .left = 0, .top = 0, .right = 1920, .bottom = 1080};

INTERNAL Quad quad2 = {.color = {1.0f, 0.0f, 1.0f, 1.0f},
          .left = 100, .top = 400, .right = 1000, .bottom = 700};

INTERNAL Quad quad3 = {.color = {0.0f, 1.0f, 0.0f, 1.0f},
          .left = 400, .top = 500, .right = 700, .bottom = 600};

INTERNAL void onCreate() {
    SDL_Log("IntroState onCreate");



    TextureAtlas_Create(&atlas, 4096, 4096, PBF_RGBA);
    u32 tex1 = TextureAtlas_AddImageFromPNG(&atlas, "assets/sample.png");
    u32 tex2 = TextureAtlas_AddImageFromPNG(&atlas, "assets/brick.png");
    TextureAtlas_PackAndUpload(&atlas);

    atlasquad1.atlasId = tex1;
    atlasquad2.atlasId = tex2;
}

INTERNAL void onDestroy() {
    SDL_Log("IntroState onDestroy");
    TextureAtlas_Destroy(&atlas);
}

INTERNAL void onStart() {
    SDL_Log("IntroState onStart");
}

INTERNAL void onStop() {
    SDL_Log("IntroState onStop");
}

INTERNAL void onFrame(RenderCmdBuffer* renderBuffer, double frameDelta) {
    //SDL_Log("IntroState onFrame");

    Render_PushQuadsCmd(renderBuffer, &quad1, 1);

    Render_PushAtlasQuadsCmd(renderBuffer, &atlas, &atlasquad1, 1);
    Render_PushAtlasQuadsCmd(renderBuffer, &atlas, &atlasquad2, 1);

    Render_PushQuadsCmd(renderBuffer, &quad2, 1);
    Render_PushQuadsCmd(renderBuffer, &quad3, 1);

    //Render_PushTexturedQuadsCmd(&renderBuffer, fontAtlas.textureId, &texquad1, 1);

    //Render_PushText(&renderBuffer, &font, 5, 25, COLOR_WHITE, "SYSTEM READY. Doctor Yeti, tag en slapper!###¤");

    //Render_PushAtlasQuadsCmd(&renderBuffer, &atlas, &atlasquad1, 1);
    //Render_PushAtlasQuadsCmd(&renderBuffer, &atlas, &atlasquad2, 1);
}

void IntroState_Register() {
    GameState_Create(GAME_STATE_INTRO);
    GameState_BindOnCreate(GAME_STATE_INTRO, onCreate);
    GameState_BindOnDestroy(GAME_STATE_INTRO, onDestroy);
    GameState_BindOnStart(GAME_STATE_INTRO, onStart);
    GameState_BindOnStop(GAME_STATE_INTRO, onStop);
    GameState_BindOnFrame(GAME_STATE_INTRO, onFrame);
}

