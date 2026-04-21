#include "RobotBase.h"
#include <cstdlib>
#include <ctime>
#include <set>
#include <cmath>
#include <limits>
#include <utility>
#include <iostream>

class Robot_Flame_e_o : public RobotBase
{
private:
    bool target_found = false;
    int target_row = -1;
    int target_col = -1;
    int radar_direction = 0;  // Start with 360 scan
    const int max_range = 4;
    std::set<std::pair<int, int>> obstacles_memory;
    
    int calculate_distance(int row1, int col1, int row2, int col2) const
    {
        return std::abs(row1 - row2) + std::abs(col1 - col2);
    }
    
    void find_closest_enemy(const std::vector<RadarObj>& radar_results, int current_row, int current_col)
    {
        target_found = false;
        int closest_distance = std::numeric_limits<int>::max();
        
        for (const auto& obj : radar_results) {
            if (obj.m_type == 'R') {
                int distance = calculate_distance(current_row, current_col, obj.m_row, obj.m_col);
                if (distance < closest_distance) {
                    closest_distance = distance;
                    target_row = obj.m_row;
                    target_col = obj.m_col;
                    target_found = true;
                }
            }
        }
    }
    
    bool is_passable(int row, int col) const
    {
        return obstacles_memory.find({row, col}) == obstacles_memory.end();
    }

public:
    Robot_Flame_e_o() : RobotBase(2, 5, flamethrower)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }
    
    virtual void get_radar_direction(int& radar_direction_out) override
    {
        // Always scan 360 degrees to find enemies
        radar_direction_out = 0;
    }
    
    virtual void process_radar_results(const std::vector<RadarObj>& radar_results) override
    {
        int current_row, current_col;
        get_current_location(current_row, current_col);
        
        for (const auto& obj : radar_results) {
            if (obj.m_type == 'M' || obj.m_type == 'P' || obj.m_type == 'F') {
                obstacles_memory.insert({obj.m_row, obj.m_col});
            }
        }
        
        find_closest_enemy(radar_results, current_row, current_col);
        
        if (target_found) {
            std::cout << "  Flame: Enemy at (" << target_row << "," << target_col << ")" << std::endl;
        }
    }
    
    virtual bool get_shot_location(int& shot_row, int& shot_col) override
    {
        if (target_found) {
            int current_row, current_col;
            get_current_location(current_row, current_col);
            int distance = calculate_distance(current_row, current_col, target_row, target_col);
            
            if (distance <= max_range) {
                shot_row = target_row;
                shot_col = target_col;
                std::cout << "  FLAMETHROWER FIRING at range " << distance << "!" << std::endl;
                target_found = false;
                return true;
            }
        }
        return false;
    }
    
    virtual void get_move_direction(int& move_direction, int& move_distance) override
    {
        int current_row, current_col;
        get_current_location(current_row, current_col);
        int max_speed = get_move_speed();
        
        if (target_found) {
            // Move TOWARD the enemy
            int row_diff = target_row - current_row;
            int col_diff = target_col - current_col;
            
            if (std::abs(row_diff) > std::abs(col_diff)) {
                move_direction = (row_diff > 0) ? 5 : 1;
            } else {
                move_direction = (col_diff > 0) ? 3 : 7;
            }
            move_distance = 1;
            std::cout << "  Moving toward enemy" << std::endl;
            return;
        }
        
        // No enemy: move in a spiral search pattern
        static int spiral_step = 0;
        static int spiral_dir = 1;
        
        spiral_step++;
        if (spiral_step > 3) {
            spiral_step = 0;
            spiral_dir = (spiral_dir % 8) + 1;
        }
        
        move_direction = spiral_dir;
        move_distance = 1;
    }
};

extern "C" RobotBase* create_robot()
{
    return new Robot_Flame_e_o();
}

extern "C" const char* robot_summary()
{
    return "Aggressive hunter: seeks and burns enemies.";
}
