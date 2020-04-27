#ifndef ENVIRONMENTOPTIONS_H
#define ENVIRONMENTOPTIONS_H
#include "gl_const.h"

class EnvironmentOptions
{
public:
    EnvironmentOptions(int ST, int KI = 2, int MT = CN_SP_MT_EUCL, int HW = 1, bool BT = true);
    EnvironmentOptions();
    int     searchtype;
    int     k;               //Can be >= 2
    int     metrictype;      //Can be chosen Euclidean, Manhattan, Chebyshev and Diagonal distance
    int     heuristicweight; //Can be >= 1
    bool    breakingties;    //0 is g-min, 1 is g-max
};

#endif // ENVIRONMENTOPTIONS_H
