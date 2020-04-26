#ifndef ENVIRONMENTOPTIONS_H
#define ENVIRONMENTOPTIONS_H
#include "gl_const.h"

class EnvironmentOptions
{
public:
    EnvironmentOptions(int ST, bool AD, bool CC, bool AS, int MT = CN_SP_MT_EUCL, int HW = 1, bool BT = true);
    EnvironmentOptions();
    int     searchtype;
    bool    allowdiagonal;   //Option that allows to make diagonal moves
    bool    cutcorners;      //Option that allows to make diagonal moves, when one adjacent cell is untraversable
    bool    allowsqueeze;    //Option that allows to move throught "bottleneck"
    int     metrictype;      //Can be chosen Euclidean, Manhattan, Chebyshev and Diagonal distance
    int     heuristicweight; //Can be >= 1
    bool    breakingties;    //0 is g-min, 1 is g-max
};

#endif // ENVIRONMENTOPTIONS_H
