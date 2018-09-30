//
// Created by bison on 9/30/18.
//

#ifndef GAME_COMBAT_H
#define GAME_COMBAT_H

#include "entity.h"
#include <vector.h>

/*
    Combat algorithm

    Combat takes place in rounds, start at round 0.

    Each round:
        1. Loop trough each combatant and calculate initiative, sort combatants by initiative to determine who starts the round
        2. Go trough each combatant in order of initiative:
        3. For AI combatant, determine and execute action
        4. For player combatant, pause simulation and wait for player to input valid action
        5. Execute player action (duration varies)
        5. Check victory condition: if only one remaining faction, end simulation, otherwise goto 1.
 */

typedef struct CombatEvent CombatEvent;
typedef struct CombatInterface CombatInterface;

typedef enum {
                        ENC_TEAM_1,
                        ENC_TEAM_2,
                        ENC_TEAM_3,
                        ENC_TEAM_4
} Team;

typedef enum {
                        ES_STOPPED,
                        ES_RUNNING,
                        ES_INPUT_WAIT,
                        ES_PAUSED,
} EncounterState;

typedef struct {
    Entity*             entity;
    bool                isPlayerControlled;
    Team                team;
    i32                 lastInitiativeRoll;

} Combatant;

typedef struct {
    i32                 round;
    Vector              combatants;
    u64                 currentTime;
    i64                 timeToNextEvent;
    EncounterState      state;
    CombatEvent*        eventStack;
    i32                 eventStackTop;
    CombatInterface*    combatInterface;
    i32                 curCombatantId;
} Encounter;

struct CombatEvent {
    u32                 (*combatEventAction)(Encounter* enc);
};

struct CombatInterface {
    void                (*onBeginRound)(Encounter* enc);
    void                (*onBeginTurn)(Encounter* enc);
    void                (*onEndTurn)(Encounter* enc);
    void                (*onEndRound)(Encounter* enc);
};

Encounter*  Encounter_Create(CombatInterface* combatInterface);
void        Encounter_Destroy(Encounter* enc);
void        Encounter_AddEntity(Encounter* enc, Entity* entity, Team team);
void        Encounter_RemoveEntity(Encounter* enc, Entity* entity);
void        Encounter_Update(Encounter* enc, u64 time_ms);
void        Encounter_Start(Encounter* enc);
void        Encounter_Pause(Encounter* enc);
void        Encounter_Resume(Encounter* enc);


#endif //GAME_COMBAT_H
