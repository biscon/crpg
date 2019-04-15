//
// Created by bison on 4/11/19.
//

#include <SDL_log.h>
#include "combat_state.h"
#include "game_state.h"
#include "../rpg/combat.h"
#include "../rpg/rpg.h"
#include "../rpg/rpg_log.h"

INTERNAL RPGContext rpgContext;
INTERNAL Encounter *encounter;
INTERNAL Entity *entity;
INTERNAL Entity *monster;
INTERNAL Entity *monster2;
INTERNAL RexImage rexImage;
INTERNAL Terminal term;
INTERNAL Terminal logTerm;
INTERNAL TextureAtlas fontAtlas;
INTERNAL TextureAtlas logFontAtlas;
INTERNAL CombatInterface interface;

/**
 * Test combat interface
 */
INTERNAL void onBeginRound(Encounter* enc)
{
    RPG_LOG("\nStarting round %d ------------------------------------------\n", enc->round);
}


INTERNAL void onCreate() {
    SDL_Log("CombatState onCreate");

    PixelBuffer pb;
    PixelBuffer_CreateFromPNG(&pb, "assets/cp437_18x18.png");

    TextureAtlas_CreateFromSheet(&fontAtlas, 18, 18, &pb);
    PixelBuffer_Destroy(&pb);


    PixelBuffer_CreateFromPNG(&pb, "assets/font9x14.png");
    TextureAtlas_CreateFromSheet(&logFontAtlas, 9, 14, &pb);
    PixelBuffer_Destroy(&pb);



    Term_Create(&term, 80, 60, 18, 18, 0, &fontAtlas);
    Term_Create(&logTerm, 53, 20, 9, 14, 0, &logFontAtlas);


    // game init
    RPG_InitContext(&rpgContext);
    EntityClass *entityClass = RPG_GetEntityClass(&rpgContext, "Fighter");
    assert(entityClass != NULL);
    entity = RPG_CreateCharacterEntity(&rpgContext, entityClass, "Schweibart", 1);
    //Entity_IncreaseLevel(entity);
    WeaponTemplate *maintpl = RPG_GetWeaponTemplate(&rpgContext, "Longsword");
    assert(maintpl != NULL);
    WeaponTemplate *offtpl = RPG_GetWeaponTemplate(&rpgContext, "Shortsword");
    assert(offtpl != NULL);
    ArmorTemplate *armortpl = RPG_GetArmorTemplate(&rpgContext, "Chain Mail");
    assert(armortpl != NULL);
    ArmorTemplate *shieldtpl = RPG_GetArmorTemplate(&rpgContext, "Shield");
    assert(shieldtpl != NULL);

    Entity_SetMainWeapon(entity, Weapon_CreateFromTemplate(maintpl));
    Entity_SetOffWeapon(entity, Weapon_CreateFromTemplate(offtpl));
    Entity_SetArmor(entity, Armor_CreateFromTemplate(armortpl));
    Entity_SetShield(entity, Armor_CreateFromTemplate(shieldtpl));
    RPG_LogEntity(entity);

    monster = RPG_CreateMonsterFromTemplate(&rpgContext, RPG_GetMonsterTemplate(&rpgContext, "Dire Wolf"), 2);
    monster2 = RPG_CreateMonsterFromTemplate(&rpgContext, RPG_GetMonsterTemplate(&rpgContext, "Swearwolf"), 1);

    RPG_LogEntity(monster);

    memset(&interface, 0, sizeof(CombatInterface));
    interface.onBeginRound = onBeginRound;

    Rex_CreateFromFile(&rexImage, "assets/map.xp");

    encounter = Encounter_Create(&interface, &rexImage);
    Encounter_AddEntity(encounter, entity, ENC_PLAYER_TEAM);
    Encounter_AddEntity(encounter, monster, ENC_ENEMY_TEAM);
    Encounter_AddEntity(encounter, monster2, ENC_ENEMY_TEAM);
    Encounter_Start(encounter);
}

INTERNAL void onDestroy() {
    SDL_Log("CombatState onDestroy");
    Encounter_Destroy(encounter);

    RPG_DestroyEntity(entity);
    RPG_DestroyEntity(monster);
    RPG_DestroyEntity(monster2);

    RPG_ShutdownContext(&rpgContext);

    Rex_Destroy(&rexImage);

    TextureAtlas_Destroy(&fontAtlas);
    Term_Destroy(&term);

    TextureAtlas_Destroy(&logFontAtlas);
    Term_Destroy(&logTerm);
}

INTERNAL void onStart() {
    SDL_Log("CombatState onStart");
}

INTERNAL void onStop() {
    SDL_Log("CombatState onStop");
}

INTERNAL void onFrame(RenderCmdBuffer* renderBuffer, double frameDelta) {
    //SDL_Log("CombatState onFrame");
    //Term_SetBGColor(&term, TERM_COL_RED);
    if(encounter->state == ES_INPUT_WAIT) {

    }
    Encounter_Update(encounter, (u64) (frameDelta * 1000.0));

    Encounter_Render(encounter, &term);
    //Term_PrintRexImage(&term, &rexImage, 0, 0);

    //Term_Print(&term, 0, 0, buf);
    Term_Render(&term, 0.0f, 0.0f, renderBuffer);

    CombatLog_Render(&encounter->combatLog, &logTerm);
    Term_Render(&logTerm, 1440.0f, 800.0f, renderBuffer);

}

void CombatState_Register() {
    GameState_Create(GAME_STATE_COMBAT);
    GameState_BindOnCreate(GAME_STATE_COMBAT, onCreate);
    GameState_BindOnDestroy(GAME_STATE_COMBAT, onDestroy);
    GameState_BindOnStart(GAME_STATE_COMBAT, onStart);
    GameState_BindOnStop(GAME_STATE_COMBAT, onStop);
    GameState_BindOnFrame(GAME_STATE_COMBAT, onFrame);
}
