#include "config.h"
#include "gl_const.h"
#include "tinyxml2.h"

Config::Config()
{
    LogParams = nullptr;
    SearchParams = nullptr;
}

Config::~Config()
{
    if (SearchParams) delete[] SearchParams;
    if (LogParams) delete[] LogParams;
}

bool Config::getConfig(const char *FileName)
{
    std::string value;
    std::stringstream stream;
    tinyxml2::XMLElement *root = nullptr, *algorithm = nullptr, *element = nullptr, *options = nullptr;

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening XML file!\n";
        return false;
    }

    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' element found in XML file!\n";
        return false;
    }

    algorithm = root->FirstChildElement(CNS_TAG_ALG);
    if (!algorithm) {
        std::cout << "Error! No '" << CNS_TAG_ALG << "' tag found in XML file!\n";
        return false;
    }

    element = algorithm->FirstChildElement(CNS_TAG_ST);
    if (!element) {
        std::cout << "Error! No '" << CNS_TAG_ST << "' tag found in XML file!\n";
        return false;
    }
    if (element->GetText())
        value = element->GetText();
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);


    if (value == CNS_SP_ST_DIJK) {
        N = 1;
        SearchParams = new double[N];
        SearchParams[CN_SP_ST] = CN_SP_ST_DIJK;
    } else {
        if (value == CNS_SP_ST_ASTAR) {
            N = 5;
            SearchParams = new double[N];
            SearchParams[CN_SP_ST] = CN_SP_ST_ASTAR;

            element = algorithm->FirstChildElement(CNS_TAG_KI);
            if (!element) {
                std::cout << "Warning! No '" << CNS_TAG_KI << "' tag found in algorithm section.\n";
                std::cout << "Value of '" << CNS_TAG_KI << "' was defined to 2.\n";
                SearchParams[CN_SP_KI] = 2;
            } else {
                stream << element->GetText();
                stream >> SearchParams[CN_SP_KI];
                stream.str("");
                stream.clear();

                if (SearchParams[CN_SP_KI] < 2) {
                    std::cout << "Warning! Value of '" << CNS_TAG_KI << "' tag is not correctly specified. Should be >= 2.\n";
                    std::cout << "Value of '" << CNS_TAG_KI << "' was defined to 2.\n";
                    SearchParams[CN_SP_KI] = 2;
                }
            }

            element = algorithm->FirstChildElement(CNS_TAG_HW);
            if (!element) {
                std::cout << "Warning! No '" << CNS_TAG_HW << "' tag found in algorithm section.\n";
                std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1.\n";
                SearchParams[CN_SP_HW] = 1;
            } else {
                stream << element->GetText();
                stream >> SearchParams[CN_SP_HW];
                stream.str("");
                stream.clear();

                if (SearchParams[CN_SP_HW] < 1) {
                    std::cout << "Warning! Value of '" << CNS_TAG_HW << "' tag is not correctly specified. Should be >= 1.\n";
                    std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1.\n";
                    SearchParams[CN_SP_HW] = 1;
                }
            }

            element = algorithm->FirstChildElement(CNS_TAG_MT);
            if (!element) {
                std::cout << "Warning! No '" << CNS_TAG_MT << "' tag found in XML file.\n";
                std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'.\n";
                SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
            } else {
                if (element->GetText())
                    value = element->GetText();
                std::transform(value.begin(), value.end(), value.begin(), ::tolower);
                if (value == CNS_SP_MT_MANH) SearchParams[CN_SP_MT] = CN_SP_MT_MANH;
                else if (value == CNS_SP_MT_EUCL) SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
                else if (value == CNS_SP_MT_DIAG) SearchParams[CN_SP_MT] = CN_SP_MT_DIAG;
                else if (value == CNS_SP_MT_CHEB) SearchParams[CN_SP_MT] = CN_SP_MT_CHEB;
                else {
                    std::cout << "Warning! Value of'" << CNS_TAG_MT << "' is not correctly specified.\n";
                    std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'\n";
                    SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
                }
            }

            element = algorithm->FirstChildElement(CNS_TAG_BT);
            if (!element) {
                std::cout << "Warning! No '" << CNS_TAG_BT << "' tag found in XML file\n";
                std::cout << "Value of '" << CNS_TAG_BT << "' was defined to 'g-max'\n";
                SearchParams[CN_SP_BT] = CN_SP_BT_GMAX;
            } else {
                value = element->GetText();
                std::transform(value.begin(), value.end(), value.begin(), ::tolower);
                if (value == CNS_SP_BT_GMIN) {
                    SearchParams[CN_SP_BT] = CN_SP_BT_GMIN;
                } else {
                    if (value == CNS_SP_BT_GMAX) {
                        SearchParams[CN_SP_BT] = CN_SP_BT_GMAX;
                    } else {
                        std::cout << "Warning! Value of '" << CNS_TAG_BT << "' is not correctly specified.\n";
                        std::cout << "Value of '" << CNS_TAG_BT << "' was defined to 'g-max'\n";
                        SearchParams[CN_SP_BT] = CN_SP_BT_GMAX;
                    }
                }
            }
        } else {
            std::cout << "Error! Value of '" << CNS_TAG_ST << "' tag (algorithm name) is not correctly specified.\n";
            std::cout << "Supported algorithm's names are: '" << CNS_SP_ST_DIJK << "', '" << CNS_SP_ST_ASTAR << "'.\n";
            return false;
        }
    }

    options = root->FirstChildElement(CNS_TAG_OPT);
    LogParams = new std::string[3];
    LogParams[CN_LP_PATH] = "";
    LogParams[CN_LP_NAME] = "";

    if (!options) {
        std::cout << "Warning! No '" << CNS_TAG_OPT << "' tag found in XML file.\n";
        std::cout << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1).\n";
        LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_SHORT_WORD;
    } else {
        element = options->FirstChildElement(CNS_TAG_LOGLVL);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_LOGLVL << "' tag found in XML file.\n";
            std::cout << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1).\n";
            LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_SHORT_WORD;
        } else {
            stream << element->GetText();
            stream >> value;
            stream.str("");
            stream.clear();
            //std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if (value == CN_LP_LEVEL_NOPE_WORD || value == CN_LP_LEVEL_NOPE_VALUE) {
                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_NOPE_WORD;
            } else {
                if (value == CN_LP_LEVEL_TINY_WORD || value == CN_LP_LEVEL_TINY_VALUE) {
                    LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_TINY_WORD;
                } else {
                    if (value == CN_LP_LEVEL_SHORT_WORD || value == CN_LP_LEVEL_SHORT_VALUE) {
                        LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_SHORT_WORD;
                    } else {
                        if (value == CN_LP_LEVEL_MEDIUM_WORD || value == CN_LP_LEVEL_MEDIUM_VALUE) {
                            LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_MEDIUM_WORD;
                        } else {
                            if (value == CN_LP_LEVEL_FULL_WORD || value == CN_LP_LEVEL_FULL_VALUE) {
                                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_FULL_WORD;
                            } else {
                                std::cout << "'" << CNS_TAG_LOGLVL << "' is not correctly specified\n";
                                std::cout << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1).\n";
                                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_SHORT_WORD;
                            }
                        }
                    }
                }
            }
            std::cout << "Log status: " << LogParams[CN_LP_LEVEL] << '\n';
        }


        element = options->FirstChildElement(CNS_TAG_LOGPATH);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_LOGPATH << "' tag found in XML file.\n";
            std::cout << "Value of '" << CNS_TAG_LOGPATH << "' tag was defined to 'current directory'.\n";
        }
        else if (!element->GetText()) {
            std::cout << "Warning! Value of '" << CNS_TAG_LOGPATH << "' tag is missing!\n";
            std::cout << "Value of '" << CNS_TAG_LOGPATH << "' tag was defined to 'current directory'.\n";
        }
        else {
            LogParams[CN_LP_PATH] = element->GetText();
        }


        element = options->FirstChildElement(CNS_TAG_LOGFN);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_LOGFN << "' tag found in XML file!\n";
            std::cout << "Value of '" << CNS_TAG_LOGFN
                      << "' tag was defined to default (original filename +'_log' + original file extension).\n";
        }
        else if (!element->GetText()) {
            std::cout << "Warning! Value of '" << CNS_TAG_LOGFN << "' tag is missing.\n";
            std::cout << "Value of '" << CNS_TAG_LOGFN
                      << "' tag was defined to default (original filename +'_log' + original file extension).\n";
        }
        else
            LogParams[CN_LP_NAME] = element->GetText();
    }
    return true;
}
