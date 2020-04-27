#include "environmentoptions.h"

EnvironmentOptions::EnvironmentOptions() {
    searchtype = CN_SP_ST_DIJK;
    k = 2;
    metrictype = CN_SP_MT_EUCL;
    heuristicweight = 1;
    breakingties = true;
}

EnvironmentOptions::EnvironmentOptions(int ST, int KI, int MT, int HW, bool BT) {
    searchtype = ST;
    k = KI;
    metrictype = MT;
    heuristicweight = HW;
    breakingties = BT;
}
