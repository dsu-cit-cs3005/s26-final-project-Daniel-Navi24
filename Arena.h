#pragma once

#include "RobotBase.h"
#include "RadarObj.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <cstdlib>
#include <ctime>

class Arena {
private:
    // ========== CONFIGURATION VARIABLES ==========
    int m_height;              // Arena height (rows)
    int m_width;               // Arena width (columns)
    int m_max_rounds;          // Maximum number of rounds before game ends
    float m_sleep_interval;    // Seconds to sleep between rounds (for live display)
    bool m_live_display;       // Whether to show live updates

    int m_num_flamethrowers;   // Number of F obstacles to place
    int m_num_pits;            // Number of P obstacles to place
    int m_num_mounds;          // Number of M obstacles to place

    // ========== GAME STATE ==========
    std::vector<std::vector<char>> m_board;  // 2D board: '.' empty, 'R' robot, 'X' dead robot, 'M','P','F' obstacles
    std::vector<RobotBase*> m_robots;        // Pointers to all loaded robots (alive and dead)
    std::vector<bool> m_is_alive;            // Track which robots are still alive
    std::vector<std::string> m_robot_names;  // Names of robots for display
    std::vector<char> m_robot_symbols;       // Unique symbols for each robot (@, #, $, !, &, etc.)
    std::vector<void*> m_lib_handles;        // Store dlopen handles for cleanup

    int m_current_round;        // Current round number
    bool m_game_over;           // Game ended flag
    int m_winner_index;         // Index of winning robot (-1 if none yet)
    
    // Stalemate detection
    int m_rounds_without_damage;
    int m_last_health_sum;

    // ========== WEAPON DAMAGE RANGES ==========
    static constexpr int RAILGUN_MIN_DAMAGE = 10;
    static constexpr int RAILGUN_MAX_DAMAGE = 20;
    static constexpr int HAMMER_MIN_DAMAGE = 50;
    static constexpr int HAMMER_MAX_DAMAGE = 60;
    static constexpr int GRENADE_MIN_DAMAGE = 10;
    static constexpr int GRENADE_MAX_DAMAGE = 40;
    static constexpr int FLAMETHROWER_MIN_DAMAGE = 30;
    static constexpr int FLAMETHROWER_MAX_DAMAGE = 50;

    // ========== WEAPON RANGES (distance) ==========
    static constexpr int FLAMETHROWER_RANGE = 4;      // 4 cells distance
    static constexpr int HAMMER_RANGE = 1;            // Adjacent only
    static constexpr int GRENADE_AOE_SIZE = 3;        // 3x3 area

public:
    // ========== CONSTRUCTORS & DESTRUCTOR ==========
    Arena(const std::string& config_file);
    ~Arena();

    // ========== INITIALIZATION METHODS ==========
    bool loadConfig(const std::string& filename);
    void loadRobots();              // Find and load all Robot_*.cpp from robots/ directory
    void placeObstacles();          // Place M, P, F obstacles randomly
    void placeRobots();             // Place robots randomly on empty cells
    void initBoard();               // Initialize empty board with '.'

    // ========== MAIN GAME LOOP ==========
    void runGame();                 // Main game loop
    bool checkWinner();             // Check if only one robot remains alive
    void checkStalemate();          // Check for stalemate (no damage for many rounds)
    void displayArena();            // Print current arena state
    void displayRobotStats();       // Print all robots' stats

    // ========== RADAR SYSTEM ==========
    void performRadar(RobotBase* robot, int direction, std::vector<RadarObj>& results);
    void scanDirection(int robot_row, int robot_col, int direction, std::vector<RadarObj>& results);
    void scanSurrounding(int robot_row, int robot_col, std::vector<RadarObj>& results);
    bool isValidCell(int row, int col);
    char getCellType(int row, int col);

    // ========== MOVEMENT SYSTEM ==========
    void handleMove(RobotBase* robot, int direction, int distance);
    bool canMoveTo(int row, int col, int& collision_type);  // collision_type: 0=empty, 1=robot, 2=mound, 3=pit, 4=flamethrower
    void applyMovementCollision(RobotBase* robot, int final_row, int final_col, int collision_type);

    // ========== SHOOTING SYSTEM ==========
    void handleShot(RobotBase* shooter, int target_row, int target_col);
    void railgunShot(RobotBase* shooter, int target_row, int target_col);
    void flamethrowerShot(RobotBase* shooter, int target_row, int target_col);
    void grenadeShot(RobotBase* shooter, int target_row, int target_col);
    void hammerShot(RobotBase* shooter, int target_row, int target_col);

    // ========== DAMAGE SYSTEM ==========
    int calculateDamage(WeaponType weapon);
    void applyDamage(RobotBase* target, int damage);
    void applyDamageToCell(int row, int col, int damage, RobotBase* shooter);

    // ========== UTILITY METHODS ==========
    int getRandomNumber(int min, int max);
    void cleanupRobots();           // Delete dynamically allocated robots
    void sleepIfLive();             // Sleep if live display is enabled
    void markRobotDead(int index);  // Mark robot as dead, update board to 'X'
    int findRobotAtPosition(int row, int col);  // Returns index of robot at position, -1 if none

    // ========== COMPILATION & LOADING HELPERS ==========
    std::string compileRobot(const std::string& cpp_file);
    RobotBase* loadRobot(const std::string& shared_lib, void*& handle);
    void findRobotFiles(std::vector<std::string>& robot_files);
};

// ========== GLOBAL HELPER FUNCTIONS ==========
std::string getRobotName(const std::string& cpp_file);
char getNextRobotSymbol(int index);
