// RobotWarz.cpp
// Main executable for the RobotWarz game
// Usage: ./RobotWarz <config_file>

#include "Arena.h"
#include <iostream>
#include <cstdlib>

void printBanner() {
    std::cout << "\n";
    std::cout << "  ****************************************\n";
    std::cout << "  *                                      *\n";
    std::cout << "  *           R O B O T W A R Z          *\n";
    std::cout << "  *                                      *\n";
    std::cout << "  *    May the best robot win!          *\n";
    std::cout << "  *                                      *\n";
    std::cout << "  ****************************************\n";
    std::cout << "\n";
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <config_file>\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << program_name << " config.txt\n";
    std::cout << "\nConfig file format:\n";
    std::cout << "  Arena_Size:Height Width\n";
    std::cout << "  Max_Rounds:10000\n";
    std::cout << "  Sleep_interval:0.5\n";
    std::cout << "  Game_State_Live:true\n";
    std::cout << "  Flamethrowers:5\n";
    std::cout << "  Pits:5\n";
    std::cout << "  Mounds:5\n";
}

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 2) {
        std::cerr << "Error: Missing config file argument!\n\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // Print welcome banner
    printBanner();
    
    std::string config_file = argv[1];
    std::cout << "Loading configuration from: " << config_file << "\n\n";
    
    // Create and run the arena
    try {
        Arena arena(config_file);
        arena.runGame();
    } catch (const std::exception& e) {
        std::cerr << "\nFatal error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nThanks for playing RobotWarz!\n";
    return 0;
}
