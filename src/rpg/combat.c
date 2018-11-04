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
 * Internal structures
 */

struct Path {
 ListNode*  stepList; // List of Position structs
 i32        steps;
 i32        movesLeft;
};


/*
 * Internal functions
 */
INTERNAL i32 FindCombatantIndexByEntity(Encounter *enc, Entity* entity)
{
    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, i);
        if(c->entity == entity)
            return i;
    }
    return -1;
}

INTERNAL void PushCombatEvent(Encounter* enc, const CombatEvent* event)
{
    if(enc->eventStackTop+1 >= RPG_COMBAT_EVENT_STACK_SIZE) {
        RPG_LOG("Combat event stack full, refusing event\n");
        return;
    }
    enc->eventStackTop++;
    memcpy(((void *) enc->eventStack)+(enc->eventStackTop* sizeof(CombatEvent)), event, sizeof(CombatEvent));
}

INTERNAL bool PopCombatEvent(Encounter* enc, CombatEvent *dst_event)
{
    if(enc->eventStackTop < 0) {
        return false;
    }
    memcpy(dst_event, ((void *) enc->eventStack)+(enc->eventStackTop* sizeof(CombatEvent)), sizeof(CombatEvent));
    enc->eventStackTop--;
    return true;
}

INTERNAL i32 InitiativeSortComparator(const void *p1, const void *p2)
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
inline INTERNAL i32 GetDistanceSquared(const Position* c1, const Position* c2)
{
    return (c2->x-c1->x)*(c2->x-c1->x) +
           (c2->y-c1->y)*(c2->y-c1->y);
}

// take the sqrt of me to get the actual distance ;) however that is much slower :D
inline INTERNAL i32 GetDistance(const Position* c1, const Position* c2)
{
    return abs((i32) sqrt((c2->x-c1->x)*(c2->x-c1->x) +
           (c2->y-c1->y)*(c2->y-c1->y)));
}

INTERNAL void FindClosestEnemy(Encounter* enc, const Combatant* c, DistQueryResult *result)
{
    result->closest = NULL;
    result->distance = INT32_MAX;
    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *cur = VECTOR_GET(enc->combatants, Combatant*, i);
        if(cur != c && c->team != cur->team) {  // if its not ourself and a different team
            i32 dist = abs((i32) sqrt(GetDistanceSquared(&c->position, &cur->position)));
            //RPG_LOG("Distance between %s and %s is %d\n", c->entity->name, cur->entity->name, dist);
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
INTERNAL void GetNodeNeighbors(ASNeighborList neighbors, void *node, void *context)
{
    Encounter* enc = (Encounter*) context;
    Position* pos = (Position*) node;
    i32 gx, gy;
    //RPG_LOG("GetNodeNeighbors query node at %d,%d\n", pos->x, pos->y);
    if(pos->x >= 1) {   // add left
        //RPG_LOG("Adding left %d,%d\n", pos->x-1, pos->y);
        gx = pos->x-1;
        gy = pos->y;
        if(enc->grid[gx][gy] == NULL)   // check that the tile isn't occupied
            ASNeighborListAdd(neighbors, &enc->nodeGrid[gx][gy], 1.0f);
    }
    if(pos->x <= RPG_GRID_W-2) {    // add right
        //RPG_LOG("Adding right %d,%d\n", pos->x+1, pos->y);
        gx = pos->x+1;
        gy = pos->y;
        if(enc->grid[gx][gy] == NULL)
            ASNeighborListAdd(neighbors, &enc->nodeGrid[gx][gy], 1.0f);
    }
    if(pos->y >= 1) {   // add top
        //RPG_LOG("Adding top %d,%d\n", pos->x, pos->y-1);
        gx = pos->x;
        gy = pos->y-1;
        if(enc->grid[gx][gy] == NULL)
            ASNeighborListAdd(neighbors, &enc->nodeGrid[gx][gy], 1.0f);
    }
    if(pos->y <= RPG_GRID_H-2) {   // add bottom
        //RPG_LOG("Adding bottom %d,%d\n", pos->x, pos->y+1);
        gx = pos->x;
        gy = pos->y+1;
        if(enc->grid[gx][gy] == NULL)
            ASNeighborListAdd(neighbors, &enc->nodeGrid[gx][gy], 1.0f);
    }
}

INTERNAL float GetPathCostHeuristic(void *fromNode, void *toNode, void *context)
{
    Position* from = (Position*) fromNode;
    Position* to = (Position*) toNode;
    float res = (float) fabs(sqrt(GetDistanceSquared(from, to)));
    //RPG_LOG("Returning distance heuristic (%d,%d --> %d,%d) %f\n", from->x, from->y, to->x, to->y, res);
    return res;
}

// TODO this only finds free tiles immediately around an enemy, to support moving partially
// towards enemies in a turn it would be smarter to find an incomplete path with a-star
INTERNAL void FindClosestAdjacentUnoccupiedPosition(Encounter* enc, Combatant* attacker,
                                                    Combatant* target, Position* result)
{
    Position *pos = &target->position;
    i32 gx, gy;
    // go through each sides, if occupied find distance and choose the largest
    // this is faster than doing jamming them in a container and sorting :)
    i32 mindist = INT32_MAX;
    Position minpos = {-1, -1};

    if(pos->x >= 1) {   // left
        gx = pos->x-1;
        gy = pos->y;
        if(enc->grid[gx][gy] == NULL) {   // check that the tile isn't occupied
            Position target_pos = {.x = gx, .y = gy};
            i32 dist = GetDistance(&target_pos, &attacker->position);
            //RPG_LOG("left %d,%d distance %d\n", gx, gy, dist);
            if(dist < mindist) {
                minpos = target_pos;
                mindist = dist;
            }
        }
    }
    if(pos->x <= RPG_GRID_W-2) {    // right
        gx = pos->x+1;
        gy = pos->y;
        if(enc->grid[gx][gy] == NULL) {
            Position target_pos = {.x = gx, .y = gy};
            i32 dist = GetDistance(&target_pos, &attacker->position);
            //RPG_LOG("right %d,%d distance %d\n", gx, gy, dist);
            if(dist < mindist) {
                minpos = target_pos;
                mindist = dist;
            }
        }
    }
    if(pos->y >= 1) {   // top
        gx = pos->x;
        gy = pos->y-1;
        if(enc->grid[gx][gy] == NULL) {
            Position target_pos = {.x = gx, .y = gy};
            i32 dist = GetDistance(&target_pos, &attacker->position);
            //RPG_LOG("top %d,%d distance %d\n", gx, gy, dist);
            if(dist < mindist) {
                minpos = target_pos;
                mindist = dist;
            }
        }
    }
    if(pos->y <= RPG_GRID_H-2) {   // bottom
        gx = pos->x;
        gy = pos->y+1;
        if(enc->grid[gx][gy] == NULL) {
            Position target_pos = {.x = gx, .y = gy};
            i32 dist = GetDistance(&target_pos, &attacker->position);
            //RPG_LOG("bottom %d,%d distance %d\n", gx, gy, dist);
            if(dist < mindist) {
                minpos = target_pos;
                mindist = dist;
            }
        }
    }
    if(mindist != INT32_MAX) {
        result->x = minpos.x;
        result->y = minpos.y;
    } else {
        result->x = INT32_MAX;
        result->y = INT32_MAX;
    }
}

INTERNAL Path* CreatePathBetweenPositions(Encounter *enc, Position *p1, Position *p2)
{
    ASPathNodeSource source;
    memset(&source, 0, sizeof(ASPathNodeSource));
    source.nodeSize = sizeof(Position*);
    source.nodeNeighbors = GetNodeNeighbors;
    source.pathCostHeuristic = GetPathCostHeuristic;
    void *from = &enc->nodeGrid[p1->x][p1->y];
    void *to = &enc->nodeGrid[p2->x][p2->y];
    ASPath path = ASPathCreate(&source, enc, from, to);
    if(path) {
        RPG_LOG("A path was found between %d,%d and %d,%d (steps: %d)\n", p1->x, p1->y, p2->x, p2->y, ASPathGetCount(path));
        Path *result = calloc(1, sizeof(Path));
        result->steps = (i32) ASPathGetCount(path);
        for(i32 i = result->steps-1; i > 0 ; i--) {
            Position* oldnode = (Position*) ASPathGetNode(path, (size_t) i);
            Position* newnode = calloc(1, sizeof(Position));
            newnode->x = oldnode->x;
            newnode->y = oldnode->y;
            LIST_PUSH(result->stepList, newnode);
            RPG_LOG("\t\tStep %d:\t%d,%d\n", i, newnode->x, newnode->y);
        }
        ASPathDestroy(path);
        return result;
    } else {
        RPG_LOG("Pathfinding failed for reasons unknown :/\n");
    }
    return NULL;
}

INTERNAL void DestroyPath(Path* path)
{
    ListNode* cur = path->stepList;
    while(cur != NULL) {
        free(cur->data);
        cur = cur->next;
    }
    LIST_DESTROY(path->stepList);
    free(path);
}
/*
 * Action implementations
 */

INTERNAL u32 Action_BeginRound(Encounter* enc, CombatEvent* event);
INTERNAL u32 Action_BeginTurn(Encounter *enc, CombatEvent* event);

INTERNAL u32 Action_EndRound(Encounter* enc, CombatEvent* event) {
    RPG_LOG("Running Action_EndRound(), time is %d\n", enc->currentTime);
    if(enc->combatInterface->onEndRound)
        enc->combatInterface->onEndRound(enc);

    CombatEvent next_event = {.action = Action_BeginRound};
    PushCombatEvent(enc, &next_event);
    return 1000;
}

INTERNAL u32 Action_EndTurn(Encounter *enc, CombatEvent* event)
{
    RPG_LOG("Running Action_EndTurn(), time is %d\n", enc->currentTime);
    Combatant *c = VECTOR_GET(enc->combatants, Combatant*, enc->curCombatantId);
    assert(c != NULL);
    RPG_LOG("Ending turn for %s\n", c->entity->name);

    if(enc->combatInterface->onEndTurn)
        enc->combatInterface->onEndTurn(enc);


    // destroy current combatants attack lists
    LIST_DESTROY(c->attackList);
    LIST_DESTROY(c->usedAttackList);

    enc->curCombatantId++;
    // if there are still combatants to process this turn queue another event
    if(enc->curCombatantId < VECTOR_SIZE(enc->combatants)) {
        CombatEvent next_event = {.action = Action_BeginTurn};
        PushCombatEvent(enc, &next_event);
    }
    else
    {
        CombatEvent next_event = {.action = Action_EndRound};
        PushCombatEvent(enc, &next_event);
    }
    return 1000;
}

INTERNAL u32 Action_BeginTurn(Encounter *enc, CombatEvent* event) {
    RPG_LOG("Running Action_BeginTurn(), time is %d\n", enc->currentTime);
    Combatant *c = VECTOR_GET(enc->combatants, Combatant*, enc->curCombatantId);
    assert(c != NULL);
    RPG_LOG("Beginning turn for %s\n", c->entity->name);
    if (enc->combatInterface->onBeginTurn)
        enc->combatInterface->onBeginTurn(enc);

    // Create attack list
    for(i32 i = 0; i < c->entity->attackCount; ++i) {
        LIST_PUSH(c->attackList, c->entity->attacks[i]);
    }

    ListNode *cur = c->attackList;
    while(cur != NULL) {
        RPG_LOG("\tAttack: %s\n", ((Attack*) cur->data)->name);
        cur = cur->next;
    }
    /*
    if(c->team == ENC_PLAYER_TEAM) {
        enc->state = ES_INPUT_WAIT;
        RPG_LOG("Entity %s waiting on input...\n", c->entity->name);
    } else { */
        AIInterface *ai = c->aiInterface;
        assert(ai != NULL);
        ai->onDecideAction(enc, c);
        /*
        if(c->target) {
            RPG_LOG("Combatant %s selected target %s\n", c->entity->name, c->target->entity->name);
            CombatEvent next_event = {Action_EndTurn};
            PushCombatEvent(enc, &next_event);
        } else {
            RPG_LOG("Combatant %s didn't select a target, ending turn.\n", c->entity->name);
            CombatEvent next_event = {Action_EndTurn};
            PushCombatEvent(enc, &next_event);
        }
         */
    //}
    return 1000;
}

INTERNAL u32 Action_BeginRound(Encounter* enc, CombatEvent* event)
{
    RPG_LOG("Running Action_BeginRound(), time is %d\n", enc->currentTime);
    enc->round++;
    if(enc->combatInterface)
        enc->combatInterface->onBeginRound(enc);

    for(i32 i = 0; i < VECTOR_SIZE(enc->combatants); ++i) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, i);
        c->hasMovedRound = false;
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

    CombatEvent next_event = {.action = Action_BeginTurn};
    PushCombatEvent(enc, &next_event);

    return 1000;
}

INTERNAL u32 Action_Attack(Encounter *enc, CombatEvent* event)
{
    Combatant *c = VECTOR_GET(enc->combatants, Combatant*, enc->curCombatantId);
    assert(c != NULL);
    Combatant *target = event->target;
    assert(target != NULL);
    Attack* attack = event->attack;
    assert(attack != NULL);
    RPG_LOG("Combatant %s is attacking %s with %s\n", c->entity->name, target->entity->name, attack->name);

    // decide next action
    AIInterface *ai = c->aiInterface;
    assert(ai != NULL);
    ai->onDecideAction(enc, c);

    return 1000;
}

INTERNAL u32 Action_Move(Encounter *enc, CombatEvent* event)
{
    Combatant *c = VECTOR_GET(enc->combatants, Combatant*, enc->curCombatantId);
    assert(c != NULL);
    assert(c->curPath != NULL);
    // mark that combatant has moved this turn
    c->hasMovedRound = true;

    Position* step = LIST_POP(c->curPath->stepList, Position*);
    if(step) {
        RPG_LOG("Moving %s to %d,%d\n", c->entity->name, step->x, step->y);
        assert(step->x >= 0 && step->x < RPG_GRID_W && step->y >= 0 && step->y < RPG_GRID_H);
        c->position.x = step->x;
        c->position.y = step->y;
        enc->grid[step->x][step->y] = c;
        c->curPath->movesLeft--;
        free(step);
        if(c->curPath->movesLeft == 0) {
            RPG_LOG("Combatant %s has used up his moves\n", c->entity->name);
            DestroyPath(c->curPath);
            c->curPath = NULL;
            AIInterface *ai = c->aiInterface;
            assert(ai != NULL);
            ai->onDecideAction(enc, c);
        } else {
            CombatEvent next_event = {.action = Action_Move};
            PushCombatEvent(enc, &next_event);
        }
    } else {
        RPG_LOG("Combatant %s finished moving\n", c->entity->name);
        DestroyPath(c->curPath);
        c->curPath = NULL;
        AIInterface *ai = c->aiInterface;
        assert(ai != NULL);
        ai->onDecideAction(enc, c);
    }
    return 1000;
}

/*
 * Default AI implementation
 */
/*
 * 1. Add attacks to unused attacks list
 * 2. Find maximum range of any attack, in unused attack list
 * 3. Find closest enemy
 * 4. If no attacks can hit enemy, move closer (if not having already moved this round)
 * 5. Perform attack
 * 6. Add attack to used list
 * 7. if no more attacks, end turn else goto 2
 */

INTERNAL Attack* FindMaxRangeAttack(Combatant* c)
{
    ListNode* cur = c->attackList;
    i32 value_of_max = INT32_MIN;
    Attack* max_atk = NULL;
    while(cur != NULL) {
        Attack *attack = (Attack*) cur->data;
        if(attack->range > value_of_max) {
            value_of_max = attack->range;
            max_atk = attack;
        }
        cur = cur->next;
    }
    return max_atk;
}

INTERNAL void DefaultAI_OnDecideAction(Encounter *enc, Combatant *combatant)
{
    Attack *attack = FindMaxRangeAttack(combatant);
    if(attack == NULL) {
        RPG_LOG("Combatant %s has used all its attacks\n", combatant->entity->name);
        CombatEvent event = {.action = Action_EndTurn};
        PushCombatEvent(enc, &event);
        return;
    }
    assert(attack != NULL);
    DistQueryResult result;
    FindClosestEnemy(enc, combatant, &result);
    if(result.closest) {
        Combatant* target = result.closest;
        // can we hit the closest enemy with any attack?
        //RPG_LOG("Comparing DISTANCE %d >= %d (attack: %s)\n", attack->range, result.distance, attack->name);
        if(attack->range >= result.distance) {
            // push attack event & and add to used list
            /*
            RPG_LOG("Combatant %s can hit enemy %s, performing attack %s\n",
                    combatant->entity->name,
                    target->entity->name,
                    attack->name);
            */
            CombatEvent event = {.action = Action_Attack, .attack = attack, .target = target};
            LIST_REMOVE(combatant->attackList, attack);
            LIST_PUSH(combatant->usedAttackList, attack);
            PushCombatEvent(enc, &event);
        } else {
            if(combatant->hasMovedRound) {
                RPG_LOG("Combatant %s has moved this turn already, ending turn\n", combatant->entity->name);
                CombatEvent event = {.action = Action_EndTurn};
                PushCombatEvent(enc, &event);
                return;
            }

            RPG_LOG("Combatant %s can't hit enemy and must move closer.\n", combatant->entity->name);
            // create a path to the nearest enemy and move closer
            Position pos;
            FindClosestAdjacentUnoccupiedPosition(enc, combatant, target, &pos);
            if(pos.x != INT32_MAX && pos.y != INT32_MAX) {
                RPG_LOG("Found closest unoccupied position to enemy %s at %d,%d\n", target->entity->name, pos.x, pos.y);
                Path* path = CreatePathBetweenPositions(enc, &combatant->position, &pos);
                if(path) {
                    RPG_LOG("Path created to %d,%d, moving...\n", pos.x, pos.y);
                    path->movesLeft = combatant->movesPerTurn;
                    combatant->curPath = path;
                    CombatEvent event = {.action = Action_Move};
                    PushCombatEvent(enc, &event);
                } else {
                    RPG_LOG("Combatant %s can't path to %d,%d, ending turn\n", combatant->entity->name, pos.x, pos.y);
                    CombatEvent event = {.action = Action_EndTurn};
                    PushCombatEvent(enc, &event);
                }
            } else {
                RPG_LOG("Combatant %s could not find a position to move to, ending turn\n", combatant->entity->name);
                CombatEvent event = {.action = Action_EndTurn};
                PushCombatEvent(enc, &event);
            }
        }
    } else {
        RPG_LOG("Combatant %s couldn't find an enemy to attack, ending turn\n", combatant->entity->name);
        CombatEvent event = {.action = Action_EndTurn};
        PushCombatEvent(enc, &event);
    }
}

INTERNAL void DefaultAI_OnAttack(Encounter* enc, Combatant *combatant)
{
    /*
    Attack *attack = Entity_GetMaxRangedAttack(combatant->entity);
    assert(attack != NULL);
    // do we have a ranged attack?
    if(attack->range > 1) {
        RPG_LOG("Combatant %s can do a ranged attack (range: %d)\n", combatant->entity->name, attack->range);
    } else {
        RPG_LOG("Combatant %s can do a melee attack\n", combatant->entity->name);
    }
    CombatEvent event = {.action = Action_EndTurn};
    PushCombatEvent(enc, &event);
    */
}

/*
 * Implementation of external interface
 */

Encounter *Encounter_Create(CombatInterface* combatInterface, RexImage* maptemplate)
{
    Encounter *enc = calloc(1, sizeof(Encounter));
    VectorAlloc(&enc->combatants);
    enc->eventStack = calloc(RPG_COMBAT_EVENT_STACK_SIZE, sizeof(CombatEvent));
    enc->eventStackTop = -1;
    enc->combatInterface = combatInterface;
    CombatMap_CreateFromTemplate(&enc->combatMap, maptemplate);
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
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, i);
        if(c->aiInterface)
            free(c->aiInterface);
        if(c->attackList)
            LIST_DESTROY(c->attackList);
        if(c->usedAttackList)
            LIST_DESTROY(c->usedAttackList);
        free(VectorGet(&enc->combatants, i));
    }
    VECTOR_FREE(enc->combatants);
    if(enc->eventStack)
        free(enc->eventStack);
    CombatMap_Destroy(&enc->combatMap);
    free(enc);
}

void Encounter_AddEntity(Encounter *enc, Entity *entity, Team team)
{
    Combatant* combatant = calloc(1, sizeof(Combatant));
    combatant->entity = entity;
    combatant->team = team;
    combatant->aiInterface = calloc(1, sizeof(AIInterface));
    combatant->aiInterface->onAttack = DefaultAI_OnAttack;
    combatant->aiInterface->onDecideAction = DefaultAI_OnDecideAction;
    combatant->movesPerTurn = 10;
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
        if(c->attackList)
            LIST_DESTROY(c->attackList);
        if(c->usedAttackList)
            LIST_DESTROY(c->attackList);
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
        enc->timeToNextEvent = event.action(enc, &event);
    }
}

INTERNAL void PlaceHostileOnGrid(Encounter *enc, Combatant *enemy)
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

INTERNAL void PlaceFriendlyOnGrid(Encounter *enc, Combatant *c)
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
    CombatEvent event = {.action = Action_BeginRound};
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
