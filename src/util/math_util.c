//
// Created by bison on 10/13/18.
//

#include <SDL_log.h>
#include "math_util.h"

double remapDouble(double oMin, double oMax, double nMin, double nMax, double x){
//range check
    if( oMin == oMax) {
        SDL_Log("Warning: Zero input range");
        return -1;    }

    if( nMin == nMax){
        SDL_Log("Warning: Zero output range");
        return -1;        }

//check reversed input range
    bool reverseInput = false;
    double oldMin = MIN(oMin, oMax );
    double oldMax = MAX(oMin, oMax );
    if (oldMin == oMin)
        reverseInput = true;

//check reversed output range
    bool reverseOutput = false;
    double newMin = MIN(nMin, nMax );
    double newMax = MAX(nMin, nMax );
    if (newMin == nMin)
        reverseOutput = true;

    double portion = (x-oldMin)*(newMax-newMin)/(oldMax-oldMin);
    if (reverseInput)
        portion = (oldMax-x)*(newMax-newMin)/(oldMax-oldMin);

    double result = portion + newMin;
    if (reverseOutput)
        result = newMax - portion;

    return result;
}

float remapFloat(float oMin, float oMax, float nMin, float nMax, float x){
//range check
    if( oMin == oMax) {
        SDL_Log("Warning: Zero input range");
        return -1;    }

    if( nMin == nMax){
        SDL_Log("Warning: Zero output range");
        return -1;        }

//check reversed input range
    bool reverseInput = false;
    float oldMin = MIN(oMin, oMax );
    float oldMax = MAX(oMin, oMax );
    if (oldMin == oMin)
        reverseInput = true;

//check reversed output range
    bool reverseOutput = false;
    float newMin = MIN(nMin, nMax );
    float newMax = MAX(nMin, nMax );
    if (newMin == nMin)
        reverseOutput = true;

    float portion = (x-oldMin)*(newMax-newMin)/(oldMax-oldMin);
    if (reverseInput)
        portion = (oldMax-x)*(newMax-newMin)/(oldMax-oldMin);

    float result = portion + newMin;
    if (reverseOutput)
        result = newMax - portion;

    return result;
}
