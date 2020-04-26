#include "map.h"

Map::Map() {
    height = -1;
    width = -1;
    start_i = -1;
    start_j = -1;
    goal_i = -1;
    goal_j = -1;
    Grid = nullptr;
    cellSize = 1;
}

Map::~Map() {
    if (Grid) {
        for (int i = 0; i < height; ++i)
            delete[] Grid[i];
        delete[] Grid;
    }
}

bool Map::CellIsTraversable(int i, int j) const {
    return (Grid[i][j] == CN_GC_NOOBS);
}

bool Map::CellIsObstacle(int i, int j) const {
    return (Grid[i][j] != CN_GC_NOOBS);
}

bool Map::CellOnGrid(int i, int j) const {
    return (i < height && i >= 0 && j < width && j >= 0);
}

bool Map::getMap(const char *FileName) {
    int rowiter = 0, grid_i = 0, grid_j = 0;

    tinyxml2::XMLElement *root = nullptr, *map = nullptr, *element = nullptr, *mapnode;

    std::string value;
    std::stringstream stream;

    bool hasGridMem = false, hasGrid = false, hasHeight = false, hasWidth = false, hasSTX = false, hasSTY = false, hasFINX = false, hasFINY = false, hasCellSize = false;

    tinyxml2::XMLDocument doc;

    // Load XML File
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening XML file!\n";
        return false;
    }
    // Get ROOT element
    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' tag found in XML file!\n";
        return false;
    }

    // Get MAP element
    map = root->FirstChildElement(CNS_TAG_MAP);
    if (!map) {
        std::cout << "Error! No '" << CNS_TAG_MAP << "' tag found in XML file!\n";
        return false;
    }

    for (mapnode = map->FirstChildElement(); mapnode; mapnode = mapnode->NextSiblingElement()) {
        element = mapnode->ToElement();
        value = mapnode->Value();
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        stream.str("");
        stream.clear();

        if(value != CNS_TAG_GRID)
        {
           stream << element->GetText();
        }


        if (!hasGridMem && hasHeight && hasWidth) {
            Grid = new int *[height];
            for (int i = 0; i < height; ++i)
                Grid[i] = new int[width];
            hasGridMem = true;
        }

        if (value == CNS_TAG_HEIGHT) {
            if (hasHeight) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_HEIGHT << "' encountered.\n";
                std::cout << "Only first value of '" << CNS_TAG_HEIGHT << "' =" << height << "will be used.\n";
            }
            else {
                if (!((stream >> height) && (height > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_HEIGHT
                              << "' tag encountered (or could not convert to integer).\n";
                    std::cout << "Value of '" << CNS_TAG_HEIGHT << "' tag should be an integer >=0\n";
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_HEIGHT
                              << "' tag will be encountered later...\n";
                }
                else
                    hasHeight = true;
            }
        }
        else if (value == CNS_TAG_WIDTH) {
            if (hasWidth) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_WIDTH << "' encountered.\n";
                std::cout << "Only first value of '" << CNS_TAG_WIDTH << "' =" << width << "will be used.\n";
            }
            else {
                if (!((stream >> width) && (width > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_WIDTH
                              << "' tag encountered (or could not convert to integer).\n";
                    std::cout << "Value of '" << CNS_TAG_WIDTH << "' tag should be an integer AND >0\n";
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_WIDTH
                              << "' tag will be encountered later...\n";

                }
                else
                    hasWidth = true;
            }
        }
        else if (value == CNS_TAG_CELLSIZE) {
            if (hasCellSize) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_CELLSIZE << "' encountered.\n";
                std::cout << "Only first value of '" << CNS_TAG_CELLSIZE << "' =" << cellSize << "will be used.\n";
            }
            else {
                if (!((stream >> cellSize) && (cellSize > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_CELLSIZE
                              << "' tag encountered (or could not convert to double).\n";
                    std::cout << "Value of '" << CNS_TAG_CELLSIZE
                              << "' tag should be double AND >0. By default it is defined to '1'\n";
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_CELLSIZE
                              << "' tag will be encountered later...\n";
                }
                else
                    hasCellSize = true;
            }
        }
        else if (value == CNS_TAG_STX) {
            if (!hasWidth) {
                std::cout << "Error! '" << CNS_TAG_STX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag.\n";
                return false;
            }

            if (hasSTX) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STX << "' encountered.\n";
                std::cout << "Only first value of '" << CNS_TAG_STX << "' =" << start_j << "will be used.\n";
            }
            else {
                if (!(stream >> start_j && start_j >= 0 && start_j < width)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STX
                              << "' tag encountered (or could not convert to integer)\n";
                    std::cout << "Value of '" << CNS_TAG_STX << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_WIDTH << "' value, which is " << width << '\n';
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STX
                              << "' tag will be encountered later...\n";
                }
                else
                    hasSTX = true;
            }
        }
        else if (value == CNS_TAG_STY) {
            if (!hasHeight) {
                std::cout << "Error! '" << CNS_TAG_STY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag.\n";
                return false;
            }

            if (hasSTY) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STY << "' encountered.\n";
                std::cout << "Only first value of '" << CNS_TAG_STY << "' =" << start_i << "will be used.\n";
            }
            else {
                if (!(stream >> start_i && start_i >= 0 && start_i < height)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STY
                              << "' tag encountered (or could not convert to integer)\n";
                    std::cout << "Value of '" << CNS_TAG_STY << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_HEIGHT << "' value, which is " << height << '\n';
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STY
                              << "' tag will be encountered later...\n";
                }
                else
                    hasSTY = true;
            }
        }
        else if (value == CNS_TAG_FINX) {
            if (!hasWidth) {
                std::cout << "Error! '" << CNS_TAG_FINX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag.\n";
                return false;
            }

            if (hasFINX) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINX << "' encountered.\n";
                std::cout << "Only first value of '" << CNS_TAG_FINX << "' =" << goal_j << "will be used.\n";
            }
            else {
                if (!(stream >> goal_j && goal_j >= 0 && goal_j < width)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINX
                              << "' tag encountered (or could not convert to integer)\n";
                    std::cout << "Value of '" << CNS_TAG_FINX << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_WIDTH << "' value, which is " << width << '\n';
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINX
                              << "' tag will be encountered later...\n";
                }
                else
                    hasFINX = true;
            }
        }
        else if (value == CNS_TAG_FINY) {
            if (!hasHeight) {
                std::cout << "Error! '" << CNS_TAG_FINY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag.\n";
                return false;
            }

            if (hasFINY) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINY << "' encountered.\n";
                std::cout << "Only first value of '" << CNS_TAG_FINY << "' =" << goal_i << "will be used.\n";
            }
            else {
                if (!(stream >> goal_i && goal_i >= 0 && goal_i < height)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINY
                              << "' tag encountered (or could not convert to integer)\n";
                    std::cout << "Value of '" << CNS_TAG_FINY << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_HEIGHT << "' value, which is " << height << '\n';
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINY
                              << "' tag will be encountered later...\n";
                }
                else
                    hasFINY = true;
            }
        }
        else if (value == CNS_TAG_GRID) {
            hasGrid = true;
            if (!(hasHeight && hasWidth)) {
                std::cout << "Error! No '" << CNS_TAG_WIDTH << "' tag or '" << CNS_TAG_HEIGHT << "' tag before '"
                          << CNS_TAG_GRID << "'tag encountered!\n";
                return false;
            }
            element = mapnode->FirstChildElement();
            while (grid_i < height) {
                if (!element) {
                    std::cout << "Error! Not enough '" << CNS_TAG_ROW << "' tags inside '" << CNS_TAG_GRID << "' tag.\n";
                    std::cout << "Number of '" << CNS_TAG_ROW
                              << "' tags should be equal (or greater) than the value of '" << CNS_TAG_HEIGHT
                              << "' tag which is " << height << '\n';
                    return false;
                }
                std::string str = element->GetText();
                std::vector<std::string> elems;
                std::stringstream ss(str);
                std::string item;
                while (std::getline(ss, item, ' '))
                    elems.push_back(item);
                rowiter = grid_j = 0;
                int val;
                if (elems.size() > 0) {
                    for (grid_j = 0; grid_j < width; ++grid_j) {
                        if (grid_j == int(elems.size())) {
                            break;
                        }
                        stream.str("");
                        stream.clear();
                        stream << elems[size_t(grid_j)];
                        stream >> val;
                        Grid[grid_i][grid_j] = val;
                    }
                }
                if (grid_j != width) {
                    std::cout << "Invalid value on " << CNS_TAG_GRID << " in the " << grid_i + 1 << " " << CNS_TAG_ROW << '\n';
                    return false;
                }
                ++grid_i;

                element = element->NextSiblingElement();
            }
        }
    }
    //some additional checks
    if (!hasGrid) {
        std::cout << "Error! There is no tag 'grid' in xml-file!\n";
        return false;
    }
    if (!(hasFINX && hasFINY && hasSTX && hasSTY))
        return false;

    if (Grid[start_i][start_j] != CN_GC_NOOBS) {
        std::cout << "Error! Start cell is not traversable (cell's value is" << Grid[start_i][start_j] << ")!\n";
        return false;
    }

    if (Grid[goal_i][goal_j] != CN_GC_NOOBS) {
        std::cout << "Error! Goal cell is not traversable (cell's value is" << Grid[goal_i][goal_j] << ")!\n";
        return false;
    }

    return true;
}



int Map::getValue(int i, int j) const {
    if (i < 0 || i >= height)
        return -1;

    if (j < 0 || j >= width)
        return -1;

    return Grid[i][j];
}

int Map::getValue_pair(std::pair<int, int> cord) const {
    if (cord.first < 0 || cord.first >= height)
        return -1;

    if (cord.second < 0 || cord.second >= width)
        return -1;

    return Grid[cord.first][cord.second];
}

int Map::getMapHeight() const {
      return height;
}

int Map::getMapWidth() const {
      return width;
}

double Map::getCellSize() const {
      return cellSize;
}

std::pair<int, int> Map::getMapStart() const {
      return std::make_pair(start_i, start_j);
}

std::pair<int, int> Map::getMapFinish() const {
      return std::make_pair(goal_i, goal_j);
}
