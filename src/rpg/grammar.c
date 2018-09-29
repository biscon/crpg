//
// Created by bison on 9/29/18.
//

#include <defs.h>
#include "grammar.h"

const char *Grammar_GetThirdPersonSingularObjectPronoun(GrammarType type) {
    switch(type)
    {
        case GT_MALE:       return "him";
        case GT_FEMALE:     return "her";
        case GT_CREATURE:   return "it";
    }
    return NULL;
}

const char *Grammar_GetThirdPersonSingularSubjectPronoun(GrammarType type) {
    switch(type)
    {
        case GT_MALE:       return "he";
        case GT_FEMALE:     return "she";
        case GT_CREATURE:   return "it";
    }
    return NULL;
}

const char *Grammar_GetPossessivePronoun(GrammarType type) {
    switch(type)
    {
        case GT_MALE:       return "his";
        case GT_FEMALE:     return "her";
        case GT_CREATURE:   return "its";
    }
    return NULL;
}

const char *Grammar_GetReflexivePronoun(GrammarType type) {
    switch(type)
    {
        case GT_MALE:       return "himself";
        case GT_FEMALE:     return "herself";
        case GT_CREATURE:   return "itself";
    }
    return NULL;
}
