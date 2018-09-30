//
// Created by bison on 9/30/18.
//

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "combat.h"
#include "rpg_log.h"
#include "dice.h"
#include "entity.h"

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

internal u32 Action_BeginTurn(Encounter *enc)
{
    RPG_LOG("Running Action_BeginTurn(), time is %d\n", enc->currentTime);
    Combatant *c = VECTOR_GET(enc->combatants, Combatant*, enc->curCombatantId);
    assert(c != NULL);
    RPG_LOG("Beginning turn for %s\n", c->entity->name);
    if(enc->combatInterface->onBeginTurn)
        enc->combatInterface->onBeginTurn(enc);

    CombatEvent event = {Action_EndTurn};
    PushCombatEvent(enc, &event);

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
 * Implementation of external interface
 */

Encounter *Encounter_Create(CombatInterface* combatInterface)
{
    Encounter *enc = calloc(1, sizeof(Encounter));
    VectorAlloc(&enc->combatants);
    enc->eventStack = calloc(RPG_COMBAT_EVENT_STACK_SIZE, sizeof(CombatEvent));
    enc->eventStackTop = -1;
    enc->combatInterface = combatInterface;
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
    VECTOR_ADD(enc->combatants, combatant);
}

void Encounter_RemoveEntity(Encounter *enc, Entity *entity)
{
    i32 index = FindCombatantIndexByEntity(enc, entity);
    if(index > 0) {
        Combatant *c = VECTOR_GET(enc->combatants, Combatant*, index);
        VECTOR_REMOVE(enc->combatants, index);
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

void Encounter_Start(Encounter *enc)
{
    enc->timeToNextEvent = 0;
    enc->round = 0;
    CombatEvent event;
    event.combatEventAction = &Action_BeginRound;
    //for(i32 i=0; i < 10; i++)
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
