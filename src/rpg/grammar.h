//
// Created by bison on 9/29/18.
//

#ifndef GAME_GRAMMAR_H
#define GAME_GRAMMAR_H

typedef enum {
            GT_MALE,
            GT_FEMALE,
            GT_CREATURE
} GrammarType;

const char* Grammar_GetThirdPersonSingularObjectPronoun(GrammarType type); // (him/her/it)
const char* Grammar_GetThirdPersonSingularSubjectPronoun(GrammarType type); // (he/she/it)
const char* Grammar_GetPossessivePronoun(GrammarType type); // (his/her/its)
const char* Grammar_GetReflexivePronoun(GrammarType type); // (himself/herself/itself)

#endif //GAME_GRAMMAR_H
