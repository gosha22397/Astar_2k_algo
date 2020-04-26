#include "mission.h"

Mission::Mission() {
    logger = nullptr;
    fileName = nullptr;
}

Mission::Mission(const char *FileName) {
    fileName = FileName;
    logger = nullptr;
}

Mission::~Mission() {
    if (logger)
        delete logger;
}

bool Mission::getMap() {
    return map.getMap(fileName);
}

bool Mission::getConfig() {
    return config.getConfig(fileName);
}

bool Mission::createLog() {
    if (logger != nullptr) delete logger;
    logger = new XmlLogger(config.LogParams[CN_LP_LEVEL]);
    return logger->getLog(fileName, config.LogParams);
}

void Mission::createEnvironmentOptions() {
    options.searchtype = config.SearchParams[CN_SP_ST];
    options.allowdiagonal = config.SearchParams[CN_SP_AD];
    options.cutcorners = config.SearchParams[CN_SP_CC];
    options.allowsqueeze = config.SearchParams[CN_SP_AS];
    if (config.N > 4) {
        options.metrictype = config.SearchParams[CN_SP_MT];
        options.heuristicweight = config.SearchParams[CN_SP_HW];
        options.breakingties = config.SearchParams[CN_SP_BT];
    } else {
        options.metrictype = CN_SP_MT_EUCL;
        options.heuristicweight = 1;
        options.breakingties = true;
    }
}

void Mission::createSearch() {
    //might be helpful in case numerous algorithms are added
}

void Mission::startSearch() {
    if (config.LogParams[CN_LP_LEVEL] == CN_LP_LEVEL_FULL_WORD) {
        sr = search.startSearch(map, options, 2);
    } else {
        if (config.LogParams[CN_LP_LEVEL] == CN_LP_LEVEL_MEDIUM_WORD) {
            sr = search.startSearch(map, options, 1);
        } else {
            sr = search.startSearch(map, options, 0);
        }
    }
}

void Mission::printSearchResultsToConsole() {
    std::cout << "Path ";
    if (!sr.pathfound) {
        std::cout << "NOT ";
    }
    std::cout << "found!\n";
    std::cout << "numberofsteps=" << sr.numberofsteps << '\n';
    std::cout << "nodescreated=" << sr.nodescreated << '\n';
    if (sr.pathfound) {
        std::cout << "pathlength=" << sr.pathlength << '\n';
        std::cout << "pathlength_scaled=" << sr.pathlength * float(map.getCellSize()) << '\n';
    }
    std::cout << "time=" << sr.time << '\n';
}

void Mission::saveSearchResultsToLog() {
    logger->writeToLogSummary(sr.numberofsteps, sr.nodescreated, sr.pathlength, sr.time, map.getCellSize());
    if (sr.pathfound) {
        logger->writeToLogPath(sr.lppath);
        logger->writeToLogHPpath(sr.hppath);
        logger->writeToLogMap(map, sr.lppath);
        logger->writeToLogOpenClose(sr.open_close_info, sr.Node_info, sr.map_size, sr.hw, sr.first_Node);
    } else {
        logger->writeToLogNotFound();
    }
    logger->saveLog();
}

SearchResult Mission::getSearchResult() {
    return sr;
}

