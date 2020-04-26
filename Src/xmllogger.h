#ifndef XMLLOGGER_H
#define	XMLLOGGER_H
#include "ilogger.h"
#include "tinyxml2.h"

//That's the class that flushes the data to the output XML

class XmlLogger : public ILogger {

public:
    XmlLogger(std::string loglevel):ILogger(loglevel){}

    virtual ~XmlLogger() {}

    bool getLog(const char *FileName, const std::string *LogParams);

    void saveLog();

    void writeToLogMap(const Map &Map, const std::list<Node> &path);

    void writeToLogOpenClose(const std::vector<std::pair<std::set<size_t>, std::set<size_t>>>& open_close_info,
                             const std::vector<std::vector<Node>>& Node_info, size_t map_size, int hw, size_t first_Node);

    void writeToLogPath(const std::list<Node> &path);

    void writeToLogHPpath(const std::list<Node> &hppath);

    void writeToLogNotFound();

    void writeToLogSummary(unsigned long int numberofsteps, unsigned long int nodescreated, float length, double time, double cellSize);

private:
    std::string LogFileName;
    tinyxml2::XMLDocument doc;
};

#endif

