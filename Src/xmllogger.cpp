#include "xmllogger.h"
#include <iostream>

using tinyxml2::XMLElement;
using tinyxml2::XMLNode;

bool XmlLogger::getLog(const char *FileName, const std::string *LogParams) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD) {
        return true;
    }
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening input XML file\n";
        return false;
    }

    if (LogParams[CN_LP_PATH] == "" && LogParams[CN_LP_NAME] == "") {
        std::string str;
        str.append(FileName);
        size_t found = str.find_last_of(".");
        if (found != std::string::npos)
            str.insert(found, "_log");
        else
            str.append("_log");
        LogFileName.append(str);
    } else if (LogParams[CN_LP_PATH] == "") {
        LogFileName.append(FileName);
        std::string::iterator it = LogFileName.end();
        while (*it != '\\')
            it--;
        ++it;
        LogFileName.erase(it, LogFileName.end());
        LogFileName.append(LogParams[CN_LP_NAME]);
    } else if (LogParams[CN_LP_NAME] == "") {
        LogFileName.append(LogParams[CN_LP_PATH]);
        if (*(--LogParams[CN_LP_PATH].end()) != '\\') LogFileName.append("\\");
        std::string lfn;
        lfn.append(FileName);
        size_t found = lfn.find_last_of("\\");
        std::string str = lfn.substr(found);
        found = str.find_last_of(".");
        if (found != std::string::npos)
            str.insert(found, "_log");
        else
            str.append("_log");
        LogFileName.append(str);
    } else {
        LogFileName.append(LogParams[CN_LP_PATH]);
        if (*(--LogParams[CN_LP_PATH].end()) != '\\') LogFileName.append("\\");
        LogFileName.append(LogParams[CN_LP_NAME]);
    }

    XMLElement *log, *root = doc.FirstChildElement(CNS_TAG_ROOT);

    if (!root) {
        std::cout << "No '" << CNS_TAG_ROOT << "' element found in XML file\n";
        std::cout << "Can not create log\n";
        return false;
    }

    root->InsertEndChild(doc.NewElement(CNS_TAG_LOG));

    root = (root->LastChild())->ToElement();

    if (loglevel != CN_LP_LEVEL_NOPE_WORD) {
        log = doc.NewElement(CNS_TAG_MAPFN);
        log->LinkEndChild(doc.NewText(FileName));
        root->InsertEndChild(log);

        root->InsertEndChild(doc.NewElement(CNS_TAG_SUM));

        root->InsertEndChild(doc.NewElement(CNS_TAG_PATH));

        root->InsertEndChild(doc.NewElement(CNS_TAG_LPLEVEL));

        root->InsertEndChild(doc.NewElement(CNS_TAG_HPLEVEL));
    }
    if (loglevel == CN_LP_LEVEL_FULL_WORD || loglevel == CN_LP_LEVEL_MEDIUM_WORD) {
        root->InsertEndChild(doc.NewElement(CNS_TAG_LOWLEVEL));
    }
    return true;
}

void XmlLogger::saveLog() {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD) {
        return;
    }
    doc.SaveFile(LogFileName.c_str());
}

void XmlLogger::writeToLogMap(const Map &map, const std::list<Node> &path) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD || loglevel == CN_LP_LEVEL_TINY_WORD) {
        return;
    }
    XMLElement *mapTag = doc.FirstChildElement(CNS_TAG_ROOT);
    mapTag = mapTag->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_PATH);

    int iterate = 0;
    bool inPath;
    std::string str;
    for (int i = 0; i < map.getMapHeight(); ++i) {
        XMLElement *element = doc.NewElement(CNS_TAG_ROW);
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);

        for (int j = 0; j < map.getMapWidth(); ++j) {
            inPath = false;
            for(std::list<Node>::const_iterator it = path.begin(); it != path.end(); it++)
                if(it->get_i() == i && it->get_j() == j) {
                    inPath = true;
                    break;
                }
            if (!inPath)
                str += std::to_string(map.getValue(i,j));
            else
                str += CNS_OTHER_PATHSELECTION;
            str += CNS_OTHER_MATRIXSEPARATOR;
        }

        element->InsertEndChild(doc.NewText(str.c_str()));
        mapTag->InsertEndChild(element);
        str.clear();
        iterate++;
    }
}

void XmlLogger::writeToLogOpenClose(const std::vector<std::pair<std::set<size_t>, std::set<size_t>>>& open_close_info,
                                    const std::vector<std::vector<Node>>& Node_info,
                                    size_t map_size, int hw, size_t first_Node) {
    if (loglevel != CN_LP_LEVEL_FULL_WORD && loglevel != CN_LP_LEVEL_MEDIUM_WORD) {
        return;
    }
    XMLElement *lowlevel = doc.FirstChildElement(CNS_TAG_ROOT);
    lowlevel = lowlevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_LOWLEVEL);
    for (size_t now_step = 0; now_step != open_close_info.size(); ++now_step) {
        XMLElement *step = doc.NewElement(CNS_TAG_STEP);
        step->SetAttribute(CNS_TAG_ATTR_NUM, int(now_step));
        lowlevel->InsertEndChild(step);
        XMLElement* step_elem = doc.FirstChildElement(CNS_TAG_ROOT);
        step_elem = step_elem->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_LOWLEVEL)->LastChildElement(CNS_TAG_STEP);

        XMLElement *open_path = doc.NewElement(CNS_TAG_OPEN);
        step_elem->InsertEndChild(open_path);
        open_path = step_elem->FirstChildElement(CNS_TAG_OPEN);
        for (size_t number : open_close_info[now_step].first) {
            XMLElement *element = doc.NewElement(CNS_TAG_POINT);
            element->SetAttribute(CNS_TAG_ATTR_X, int(number % map_size));
            element->SetAttribute(CNS_TAG_ATTR_Y, int(number / map_size));
            element->SetAttribute(CNS_TAG_ATTR_F, Node_info[now_step].at(number).get_g() + hw * Node_info[now_step].at(number).get_h());
            element->SetAttribute(CNS_TAG_ATTR_G, Node_info[now_step].at(number).get_g());
            element->SetAttribute(CNS_TAG_ATTR_PARX, int(Node_info[now_step].at(number).parent_node % map_size));
            element->SetAttribute(CNS_TAG_ATTR_PARY, int(Node_info[now_step].at(number).parent_node / map_size));
            open_path->InsertEndChild(element);
        }

        XMLElement *close_path = doc.NewElement(CNS_TAG_CLOSE);
        step_elem->InsertEndChild(close_path);
        close_path = step_elem->FirstChildElement(CNS_TAG_CLOSE);

        for (size_t number : open_close_info[now_step].second) {
            XMLElement *element = doc.NewElement(CNS_TAG_POINT);
            element->SetAttribute(CNS_TAG_ATTR_X, int(number % map_size));
            element->SetAttribute(CNS_TAG_ATTR_Y, int(number / map_size));
            element->SetAttribute(CNS_TAG_ATTR_F, Node_info[now_step].at(number).get_g() + hw * Node_info[now_step].at(number).get_h());
            element->SetAttribute(CNS_TAG_ATTR_G, Node_info[now_step].at(number).get_g());
            if (number != first_Node) {
                element->SetAttribute(CNS_TAG_ATTR_PARX, int(Node_info[now_step].at(number).parent_node % map_size));
                element->SetAttribute(CNS_TAG_ATTR_PARY, int(Node_info[now_step].at(number).parent_node / map_size));
            }
            close_path->InsertEndChild(element);
        }
    }
}

void XmlLogger::writeToLogPath(const std::list<Node>& path) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD || loglevel == CN_LP_LEVEL_TINY_WORD || path.empty()) {
        return;
    }
    int iterate = 0;
    XMLElement *lplevel = doc.FirstChildElement(CNS_TAG_ROOT);
    lplevel = lplevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_LPLEVEL);
    for (std::list<Node>::const_iterator it = path.begin(); it != path.end(); it++) {
        XMLElement *element = doc.NewElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, it->get_j());
        element->SetAttribute(CNS_TAG_ATTR_Y, it->get_i());
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);
        lplevel->InsertEndChild(element);
        iterate++;
    }
}

void XmlLogger::writeToLogHPpath(const std::list<Node> &hppath) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD || loglevel == CN_LP_LEVEL_TINY_WORD || hppath.empty()) {
        return;
    }
    int partnumber = 0;
    XMLElement *hplevel = doc.FirstChildElement(CNS_TAG_ROOT);
    hplevel = hplevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_HPLEVEL);
    std::list<Node>::const_iterator iter = hppath.begin();
    std::list<Node>::const_iterator it = hppath.begin();

    while (iter != --hppath.end()) {
        XMLElement *part = doc.NewElement(CNS_TAG_SECTION);
        part->SetAttribute(CNS_TAG_ATTR_NUM, partnumber);
        part->SetAttribute(CNS_TAG_ATTR_STX, it->get_j());
        part->SetAttribute(CNS_TAG_ATTR_STY, it->get_i());
        ++iter;
        part->SetAttribute(CNS_TAG_ATTR_FINX, iter->get_j());
        part->SetAttribute(CNS_TAG_ATTR_FINY, iter->get_i());
        part->SetAttribute(CNS_TAG_ATTR_LENGTH, iter->g - it->g);
        hplevel->LinkEndChild(part);
        ++it;
        ++partnumber;
    }
}

void XmlLogger::writeToLogSummary(unsigned long int numberofsteps,
                                  unsigned long int nodescreated,
                                  float length, double time, double cellSize) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD) {
        return;
    }
    XMLElement *summary = doc.FirstChildElement(CNS_TAG_ROOT);
    summary = summary->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_SUM);
    XMLElement *element = summary->ToElement();
    element->SetAttribute(CNS_TAG_ATTR_NUMOFSTEPS, static_cast<unsigned int>(numberofsteps));
    element->SetAttribute(CNS_TAG_ATTR_NODESCREATED, static_cast<unsigned int>(nodescreated));
    element->SetAttribute(CNS_TAG_ATTR_LENGTH, length);
    element->SetAttribute(CNS_TAG_ATTR_LENGTH_SCALED, length*float(cellSize));
    element->SetAttribute(CNS_TAG_ATTR_TIME, std::to_string(time).c_str());
}

void XmlLogger::writeToLogNotFound() {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD) {
        return;
    }
    XMLElement *node = doc.FirstChildElement(CNS_TAG_ROOT)->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_PATH);
    node->InsertEndChild(doc.NewText("Path NOT found!"));
}
