//
// Created by bison on 9/23/18.
//

#include <string.h>
#include <malloc.h>
#include "entity_class.h"

internal void AddLevelRow(Vector *table, i32 level, i32 minxp, i32 maxxp, const char* hitdice)
{
    LevelRow *row = calloc(1, sizeof(LevelRow));
    row->level = level;
    row->minXP = minxp;
    row->maxXP = maxxp;
    strncpy(row->hitDice, hitdice, sizeof(row->hitDice));
    VectorAdd(table, row);
}

void EntityClass_InitFighter(EntityClass *entityClass)
{
    memset(entityClass, 0, sizeof(EntityClass));
    strncpy(entityClass->name, "Fighter", sizeof(entityClass->name));
    VectorAlloc(&entityClass->levelTable);
    AddLevelRow(&entityClass->levelTable,1,0,1999,"1d8");
    AddLevelRow(&entityClass->levelTable,2,2000,3999,"2d8");
    AddLevelRow(&entityClass->levelTable,3,4000,7999,"3d8");
    AddLevelRow(&entityClass->levelTable,4,8000,15999,"4d8");
    AddLevelRow(&entityClass->levelTable,5,16000,31999,"5d8");
    AddLevelRow(&entityClass->levelTable,6,32000,63999,"6d8");
    AddLevelRow(&entityClass->levelTable,7,64000, 119999,"7d8");
    AddLevelRow(&entityClass->levelTable,8,120000,239999,"8d8");
    AddLevelRow(&entityClass->levelTable,9,240000,359999,"9d8");
    AddLevelRow(&entityClass->levelTable,10,360000,479999,"9d8+2");
    AddLevelRow(&entityClass->levelTable,11,480000,599999,"9d8+4");
    AddLevelRow(&entityClass->levelTable,12,600000,719999,"9d8+6");
    AddLevelRow(&entityClass->levelTable,13,720000,839999,"9d8+8");
    AddLevelRow(&entityClass->levelTable,14,840000,959999,"9d8+10");
    AddLevelRow(&entityClass->levelTable,15,960000,1079999,"9d8+12");
    AddLevelRow(&entityClass->levelTable,16,1080000,1199999,"9d8+14");
    AddLevelRow(&entityClass->levelTable,17,1200000,1319999,"9d8+16");
    AddLevelRow(&entityClass->levelTable,18,1320000,1439999,"9d8+18");
    AddLevelRow(&entityClass->levelTable,19,1440000,1559999,"9d8+20");
    AddLevelRow(&entityClass->levelTable,20,1560000,21559999,"9d8+22");
}

void EntityClass_Free(EntityClass *entityClass) {
    for(i32 i = 0; i < VECTOR_SIZE(entityClass->levelTable); ++i) {
        free(VectorGet(&entityClass->levelTable, i));
    }
    VECTOR_FREE(entityClass->levelTable);
}

i32 EntityClass_CalcLevel(EntityClass *entityClass, i32 xp) {
    i32 size = VECTOR_SIZE(entityClass->levelTable);
    for(i32 i = 0; i < size; ++i) {
        LevelRow *row = VECTOR_GET(entityClass->levelTable, LevelRow*, i);
        if(xp >= row->minXP && xp <= row->maxXP)
            return row->level;
    }
    return size;
}

const char *EntityClass_GetHitDice(EntityClass *entityClass, i32 level) {
    if(level > 10)
        level = 10;
    i32 size = VECTOR_SIZE(entityClass->levelTable);
    for(i32 i = 0; i < size; ++i) {
        LevelRow *row = VECTOR_GET(entityClass->levelTable, LevelRow*, i);
        if(row->level == level)
            return row->hitDice;
    }
    return NULL;
}

i32 EntityClass_GetXPForLevel(EntityClass *entityClass, i32 level) {
    i32 size = VECTOR_SIZE(entityClass->levelTable);
    for(i32 i = 0; i < size; ++i) {
        LevelRow *row = VECTOR_GET(entityClass->levelTable, LevelRow*, i);
        if(row->level == level)
            return row->minXP;
    }
    // else return max level xp
    LevelRow *row = VECTOR_GET(entityClass->levelTable, LevelRow*, size-1);
    return row->minXP;
}



