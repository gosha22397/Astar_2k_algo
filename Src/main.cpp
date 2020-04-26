#include "mission.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "Error! Pathfinding task file (XML) is not specified!\n";
        return 0;
    }

    Mission mission(argv[1]);

    std::cout << argv[1] << '\n';
    std::cout << "Parsing the map from XML:\n";

    if (!mission.getMap()) {
        std::cout << "Incorrect map! Program halted!\n";
    } else {
        std::cout << "Map OK!" << '\n' << "Parsing configurations (algorithm, log) from XML:\n";
        if (!mission.getConfig()) {
            std::cout << "Incorrect configurations! Program halted!\n";
        } else {
            std::cout << "Configurations OK!" << '\n' << "Creating log channel:\n";

            if (!mission.createLog()) {
                std::cout << "Log chanel has not been created! Program halted!\n";
            } else {
                std::cout << "Log OK!"<< '\n' << "Start searching the path:\n";

                mission.createEnvironmentOptions();
                mission.createSearch();
                mission.startSearch();

                std::cout << "Search is finished!\n";

                mission.printSearchResultsToConsole();
                mission.saveSearchResultsToLog();

                std::cout << "Results are saved (if chosen) via created log channel.\n";
            }
        }
    }
    return 0;
}
