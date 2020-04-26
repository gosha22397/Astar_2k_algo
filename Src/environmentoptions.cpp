#include "environmentoptions.h"

EnvironmentOptions::EnvironmentOptions() {
    searchtype = CN_SP_ST_DIJK;
    allowdiagonal = true;
    cutcorners = false;
    allowsqueeze = false;
    metrictype = CN_SP_MT_EUCL;
    heuristicweight = 1;
    breakingties = true;
}

EnvironmentOptions::EnvironmentOptions(int ST, bool AD, bool CC, bool AS, int MT, int HW, bool BT) {
    searchtype = ST;
    allowdiagonal = AD;
    cutcorners = CC;
    allowsqueeze = AS;
    metrictype = MT;
    heuristicweight = HW;
    breakingties = BT;
}

/*
int EnvironmentOptions::get_heuristicweight() {
    return heuristicweight;
}
*/
