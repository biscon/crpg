//
// Created by bison on 9/30/18.
//

#ifndef GAME_COMBAT_H
#define GAME_COMBAT_H

#include "entity.h"
#include <vector.h>
#include <list.h>

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

        Battle arena consists of a center line, 4 slots on each side:
 */

typedef struct CombatEvent CombatEvent;
typedef struct CombatInterface CombatInterface;
typedef struct AIInterface AIInterface;
typedef struct Combatant Combatant;
typedef struct Position Position;
typedef struct Path Path;

typedef enum {
                        ENC_PLAYER_TEAM,
                        ENC_ENEMY_TEAM
} Team;

typedef enum {
                        ES_STOPPED,
                        ES_RUNNING,
                        ES_INPUT_WAIT,
                        ES_PAUSED,
} EncounterState;

struct Position {
    i32                 x;
    i32                 y;
};

typedef struct {
    Combatant *closest;
    i32 distance;
} DistQueryResult;


struct Combatant {
    Entity*             entity;
    Team                team;
    i32                 lastInitiativeRoll;
    AIInterface*        aiInterface;
    Position            position;
    ListNode*           attackList;
    ListNode*           usedAttackList;
    i32                 movesPerTurn;
    Path*               curPath;
    bool                hasMovedRound;
};

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
    Combatant*          grid[RPG_GRID_W][RPG_GRID_H];
    Position            nodeGrid[RPG_GRID_W][RPG_GRID_H];
} Encounter;

struct CombatEvent {
    u32                 (*action)    (Encounter* enc, CombatEvent* event);
    Attack*             attack;
    Combatant*          target;
};

struct CombatInterface {
    void                (*onBeginRound)         (Encounter* enc);
    void                (*onBeginTurn)          (Encounter* enc);
    void                (*onEndTurn)            (Encounter* enc);
    void                (*onEndRound)           (Encounter* enc);
};

struct AIInterface {
    void                (*onDecideAction)       (Encounter* enc, Combatant *combatant);
    void                (*onAttack)             (Encounter* enc, Combatant *combatant);
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
