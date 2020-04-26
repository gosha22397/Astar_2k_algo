#ifndef ILOGGER_H
#define	ILOGGER_H
#include "map.h"
#include "node.h"
#include <list>
#include <set>
#include <vector>

class ILogger {
    public:
        ILogger(std::string loglevel) {this->loglevel = loglevel;}
        virtual bool getLog(const char* FileName, const std::string* LogParams) = 0;
        virtual void saveLog() = 0;
        virtual void writeToLogMap(const Map& map, const std::list<Node>& path) = 0;
        virtual void writeToLogOpenClose(const std::vector<std::pair<std::set<size_t>, std::set<size_t>>>& open_close_info,
                                         const std::vector<std::vector<Node>>& Node_info, size_t map_size, int hw, size_t first_Node) = 0;
        virtual void writeToLogPath(const std::list<Node>& path) = 0;
        virtual void writeToLogHPpath(const std::list<Node>& path) = 0;
        virtual void writeToLogNotFound() = 0;
        virtual void writeToLogSummary(unsigned long int numberofsteps, unsigned long int nodescreated,
                                       float length, double time, double cellSize) = 0;
        virtual ~ILogger() {}
    protected:
        std::string loglevel;
};

#endif

