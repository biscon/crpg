//
// Created by bison on 9/30/18.
//

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "combat.h"
#include "rpg_log.h"
#include "dice.h"
#include "entity.h"
#include "attack.h"
#include "AStar.h"

/*
 * Internal functions
 */
internal i32 FindCombatantIndexByEntity(Encounter *enc, Entity* entity)
{
    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, i);
        if(c->entity == entity)
            return i;
    }
    return -1;
}

internal void PushCombatEvent(Encounter* enc, const CombatEvent* event)
{
    if(enc->eventStackTop+1 >= RPG_COMBAT_EVENT_STACK_SIZE) {
        RPG_LOG("Combat event stack full, refusing event\n");
        return;
    }
    enc->eventStackTop++;
    memcpy(((void *) enc->eventStack)+(enc->eventStackTop* sizeof(CombatEvent)), event, sizeof(CombatEvent));
}

internal bool PopCombatEvent(Encounter* enc, CombatEvent *dst_event)
{
    if(enc->eventStackTop < 0) {
        return false;
    }
    memcpy(dst_event, ((void *) enc->eventStack)+(enc->eventStackTop* sizeof(CombatEvent)), sizeof(CombatEvent));
    enc->eventStackTop--;
    return true;
}

internal i32 InitiativeSortComparator(const void *p1, const void *p2)
{
    // this is a bit fudged because our vector stores void** (and you cannot dereference void)
    const Combatant *c1 = *(Combatant**) p1;
    const Combatant *c2 = *(Combatant**) p2;
    //RPG_LOG("Comparing %d with %d\n", c1->lastInitiativeRoll, c2->lastInitiativeRoll);
    if(c1->lastInitiativeRoll == c2->lastInitiativeRoll)
        return 0;
    if(c1->lastInitiativeRoll > c2->lastInitiativeRoll)
        return -1;
    else
        return 1;
}

// take the sqrt of me to get the actual distance ;) however that is much slower :D
inline internal i32 GetDistanceSquared(const Position* c1, const Position* c2)
{
    return (c2->x-c1->x)*(c2->x-c1->x) +
           (c2->y-c1->y)*(c2->y-c1->y);
}

internal void FindClosestEnemy(Encounter* enc, const Combatant* c, DistQueryResult *result)
{
    result->closest = NULL;
    result->distance = INT32_MAX;
    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *cur = VECTOR_GET(enc->combatants, Combatant*, i);
        if(cur != c && c->team != cur->team) {  // if its not ourself and a different team
            i32 dist = abs((i32) sqrt(GetDistanceSquared(&c->position, &cur->position)));
            RPG_LOG("Distance between %s and %s is %d\n", c->entity->name, cur->entity->name, dist);
            // update result if its lower than the previous distance found O(n)
            if (dist < result->distance) {
                result->distance = dist;
                result->closest = cur;
            }
        }
    }
}

/*
 * Pathfinding
 * If introducing diagonals remember to set edge coast to 1.4f
 */
internal void GetNodeNeighbors(ASNeighborList neighbors, void *node, void *context)
{
    Encounter* enc = (Encounter*) context;
    Position* pos = (Position*) node;
    //RPG_LOG("GetNodeNeighbors query node at %d,%d\n", pos->x, pos->y);
    if(pos->x >= 1) {   // add left
        //RPG_LOG("Adding left %d,%d\n", pos->x-1, pos->y);
        ASNeighborListAdd(neighbors, &enc->nodeGrid[pos->x-1][pos->y], 1.0f);
    }
    if(pos->x <= RPG_GRID_W-2) {    // add right
        //RPG_LOG("Adding right %d,%d\n", pos->x+1, pos->y);
        ASNeighborListAdd(neighbors, &enc->nodeGrid[pos->x+1][pos->y], 1.0f);
    }
    if(pos->y >= 1) {   // add top
        //RPG_LOG("Adding top %d,%d\n", pos->x, pos->y-1);
        ASNeighborListAdd(neighbors, &enc->nodeGrid[pos->x][pos->y-1], 1.0f);
    }
    if(pos->y <= RPG_GRID_H-2) {   // add bottom
        //RPG_LOG("Adding bottom %d,%d\n", pos->x, pos->y+1);
        ASNeighborListAdd(neighbors, &enc->nodeGrid[pos->x][pos->y+1], 1.0f);
    }
}

internal float GetPathCostHeuristic(void *fromNode, void *toNode, void *context)
{
    Position* from = (Position*) fromNode;
    Position* to = (Position*) toNode;
    float res = (float) fabs(sqrt(GetDistanceSquared(from, to)));
    //RPG_LOG("Returning distance heuristic (%d,%d --> %d,%d) %f\n", from->x, from->y, to->x, to->y, res);
    return res;

}

internal void FindPathBetweenCombatants(Encounter* enc, Combatant *c1, Combatant *c2)
{
    ASPathNodeSource source;
    memset(&source, 0, sizeof(ASPathNodeSource));
    source.nodeSize = sizeof(Position*);
    source.nodeNeighbors = GetNodeNeighbors;
    source.pathCostHeuristic = GetPathCostHeuristic;
    void *from = &enc->nodeGrid[c1->position.x][c1->position.y];
    void *to = &enc->nodeGrid[c2->position.x][c2->position.y];
    ASPath path = ASPathCreate(&source, enc, from, to);
    if(path) {
        RPG_LOG("A path was found between %s and %s (steps: %d)\n", c1->entity->name, c2->entity->name, ASPathGetCount(path));
        for(i32 i = 0; i < ASPathGetCount(path); ++i) {
            Position* node = (Position*) ASPathGetNode(path, (size_t) i);
            RPG_LOG("\t\tStep %d:\t%d,%d\n", i, node->x, node->y);
        }
        ASPathDestroy(path);
    } else {
        RPG_LOG("Pathfinding failed for reasons unknown :/\n");
    }

}
/*
 * Action implementations
 */

internal u32 Action_BeginRound(Encounter* enc);
internal u32 Action_BeginTurn(Encounter *enc);

internal u32 Action_EndRound(Encounter* enc) {
    RPG_LOG("Running Action_EndRound(), time is %d\n", enc->currentTime);
    if(enc->combatInterface->onEndRound)
        enc->combatInterface->onEndRound(enc);

    CombatEvent event = {Action_BeginRound};
    PushCombatEvent(enc, &event);
    return 1000;
}

internal u32 Action_EndTurn(Encounter *enc)
{
    RPG_LOG("Running Action_EndTurn(), time is %d\n", enc->currentTime);
    Combatant *c = VECTOR_GET(enc->combatants, Combatant*, enc->curCombatantId);
    assert(c != NULL);
    RPG_LOG("Ending turn for %s\n", c->entity->name);

    if(enc->combatInterface->onEndTurn)
        enc->combatInterface->onEndTurn(enc);

    enc->curCombatantId++;
    // if there are still combatants to process this turn queue another event
    if(enc->curCombatantId < VECTOR_SIZE(enc->combatants)) {
        CombatEvent event = {Action_BeginTurn};
        PushCombatEvent(enc, &event);
    }
    else
    {
        CombatEvent event = {Action_EndRound};
        PushCombatEvent(enc, &event);
    }
    return 1000;
}

internal u32 Action_BeginTurn(Encounter *enc) {
    RPG_LOG("Running Action_BeginTurn(), time is %d\n", enc->currentTime);
    Combatant *c = VECTOR_GET(enc->combatants, Combatant*, enc->curCombatantId);
    assert(c != NULL);
    RPG_LOG("Beginning turn for %s\n", c->entity->name);
    if (enc->combatInterface->onBeginTurn)
        enc->combatInterface->onBeginTurn(enc);

    /*
    if(c->team == ENC_PLAYER_TEAM) {
        enc->state = ES_INPUT_WAIT;
        RPG_LOG("Entity %s waiting on input...\n", c->entity->name);
    } else { */
        AIInterface *ai = c->aiInterface;
        assert(ai != NULL);
        c->target = ai->onSelectTarget(enc, c);
        if(c->target) {
            RPG_LOG("Combatant %s selected target %s\n", c->entity->name, c->target->entity->name);
            CombatEvent event = {Action_EndTurn};
            PushCombatEvent(enc, &event);
        } else {
            RPG_LOG("Combatant %s didn't select a target, ending turn.\n", c->entity->name);
            CombatEvent event = {Action_EndTurn};
            PushCombatEvent(enc, &event);
        }
    //}
    return 1000;
}

internal u32 Action_BeginRound(Encounter* enc)
{
    RPG_LOG("Running Action_BeginRound(), time is %d\n", enc->currentTime);
    enc->round++;
    if(enc->combatInterface)
        enc->combatInterface->onBeginRound(enc);

    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, i);
        i32 initiative = Dice_Roll("1d6");
        initiative += Entity_GetDEXBonus(c->entity);
        c->lastInitiativeRoll = initiative;
        RPG_LOG("Combatant %s rolled initiative %d\n", c->entity->name, c->lastInitiativeRoll);
    }
    RPG_LOG("Entities sorted by initiative:\n");

    qsort(enc->combatants.items, (size_t) enc->combatants.total, sizeof(void *),
          InitiativeSortComparator);

    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, i);
        RPG_LOG("%s %d\n", c->entity->name, c->lastInitiativeRoll);
    }
    enc->curCombatantId = 0;

    CombatEvent event = {Action_BeginTurn};
    PushCombatEvent(enc, &event);

    return 1000;
}

/*
 * Default AI implementation
 */
internal Combatant* DefaultAI_OnSelectTarget(Encounter* enc, Combatant *combatant)
{
    DistQueryResult result;
    FindClosestEnemy(enc, combatant, &result);
    if(result.closest) {
        FindPathBetweenCombatants(enc, combatant, result.closest);
        return result.closest;
    }
    return NULL;
}

internal void DefaultAI_OnAttack(Encounter* enc, Combatant *combatant)
{

    Attack *attack = Entity_GetMaxRangedAttack(combatant->entity);
    assert(attack != NULL);
    // do we have a ranged attack?
    if(attack->range > 1) {
        RPG_LOG("Combatant %s can do a ranged attack (range: %d)\n", combatant->entity->name, attack->range);
    } else {
        RPG_LOG("Combatant %s can do a melee attack\n", combatant->entity->name);
    }
    CombatEvent event = {Action_EndTurn};
    PushCombatEvent(enc, &event);
}

/*
 * Implementation of external interface
 */

Encounter *Encounter_Create(CombatInterface* combatInterface)
{
    Encounter *enc = calloc(1, sizeof(Encounter));
    VectorAlloc(&enc->combatants);
    enc->eventStack = calloc(RPG_COMBAT_EVENT_STACK_SIZE, sizeof(CombatEvent));
    enc->eventStackTop = -1;
    enc->combatInterface = combatInterface;
    // initialize pathfinding node grid, could prolly be done smarter
    for(i32 x = 0; x < RPG_GRID_W; ++x) {
        for (i32 y = 0; y < RPG_GRID_H; ++y) {
            enc->nodeGrid[x][y].x = x;
            enc->nodeGrid[x][y].y = y;
        }
    }
    return enc;
}

void Encounter_Destroy(Encounter *enc)
{
    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        free(VectorGet(&enc->combatants, i));
    }
    VECTOR_FREE(enc->combatants);
    if(enc->eventStack)
        free(enc->eventStack);
    free(enc);
}

void Encounter_AddEntity(Encounter *enc, Entity *entity, Team team)
{
    Combatant* combatant = calloc(1, sizeof(Combatant));
    combatant->entity = entity;
    combatant->team = team;
    combatant->aiInterface = calloc(1, sizeof(AIInterface));
    combatant->aiInterface->onAttack = DefaultAI_OnAttack;
    combatant->aiInterface->onSelectTarget = DefaultAI_OnSelectTarget;
    VECTOR_ADD(enc->combatants, combatant);
}

void Encounter_RemoveEntity(Encounter *enc, Entity *entity)
{
    i32 index = FindCombatantIndexByEntity(enc, entity);
    if(index > 0) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, index);
        VECTOR_REMOVE(enc->combatants, index);
        if(c->aiInterface)
            free(c->aiInterface);
        free(c);
    }
}

void Encounter_Update(Encounter *enc, u64 time_ms)
{
    if(enc->state != ES_RUNNING)
        return;

    enc->currentTime += time_ms;
    enc->timeToNextEvent -= time_ms;
    if(enc->timeToNextEvent > 0)
        return;

    if(enc->eventStackTop < 0) {
        RPG_LOG("Combat event stack is empty, ending combat\n");
        enc->state = ES_STOPPED;
    } else {
        //RPG_LOG("Combat event stack top: %d\n", enc->eventStackTop);
        CombatEvent event;
        PopCombatEvent(enc, &event);
        //RPG_LOG("Took combat event off stack, running action\n");
        enc->timeToNextEvent = event.combatEventAction(enc);
    }
}

internal void PlaceHostileOnGrid(Encounter *enc, Combatant *enemy)
{
    i32 offx = RPG_GRID_W-4;
    i32 offy = (RPG_GRID_H/2)-2;
    for(i32 x = 0; x < 4; ++x) {
        for(i32 y = 0; y < 4; ++y) {
            i32 gx = offx + x;
            i32 gy = offy + y;
            //RPG_LOG("Scanning grid pos %d,%d\n", offx + x, offy + y);
            if(enc->grid[gx][gy] == NULL)
            {
                enc->grid[gx][gy] = enemy;
                enemy->position.x = gx;
                enemy->position.y = gy;
                RPG_LOG("Placing enemy combatant %s at pos %d,%d\n", enemy->entity->name, gx, gy);
                return;
            }
        }
    }
    RPG_LOG("Could not place enemy combatant %s!!\n", enemy->entity->name);
}

internal void PlaceFriendlyOnGrid(Encounter *enc, Combatant *c)
{
    i32 offx = 0;
    i32 offy = (RPG_GRID_H/2)-2;
    for(i32 x = 3; x >= 0; --x) {
        for(i32 y = 0; y < 4; ++y) {
            i32 gx = offx + x;
            i32 gy = offy + y;
            //RPG_LOG("Scanning grid pos %d,%d\n", offx + x, offy + y);
            if(enc->grid[gx][gy] == NULL)
            {
                enc->grid[gx][gy] = c;
                c->position.x = gx;
                c->position.y = gy;
                RPG_LOG("Placing combatant %s at pos %d,%d\n", c->entity->name, gx, gy);
                return;
            }
        }
    }
    RPG_LOG("Could not place combatant %s!!\n", c->entity->name);
}


void Encounter_Start(Encounter *enc)
{
    // place friendlies
    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, i);
        if(c->team == ENC_PLAYER_TEAM) {
            PlaceFriendlyOnGrid(enc, c);
        }
    }

    // place enemies
    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, i);
        if(c->team == ENC_ENEMY_TEAM) {
            PlaceHostileOnGrid(enc, c);
        }
    }

    enc->timeToNextEvent = 0;
    enc->round = 0;
    CombatEvent event;
    event.combatEventAction = &Action_BeginRound;
    PushCombatEvent(enc, &event);
    enc->state = ES_RUNNING;
}

void Encounter_Pause(Encounter *enc)
{
    enc->state = ES_PAUSED;
}

void Encounter_Resume(Encounter *enc) {
    enc->state = ES_RUNNING;
}
