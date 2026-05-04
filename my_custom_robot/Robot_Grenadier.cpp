#include "RobotBase.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

class Robot_Grenadier : public RobotBase
{
private:
    int target_row = -1;
    int target_col = -1;
    bool enemy_spotted = false;
    
public:
    Robot_Grenadier() : RobotBase(2, 5, grenade)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }
    
    virtual void get_radar_direction(int& radar_direction) override
    {
        radar_direction = 0;
    }
    
    virtual void process_radar_results(const std::vector<RadarObj>& radar_results) override
    {
        enemy_spotted = false;
        int current_row, current_col;
        get_current_location(current_row, current_col);
        
        int closest = 999;
        for (const auto& obj : radar_results) {
            if (obj.m_type == 'R') {
                int dist = std::abs(obj.m_row - current_row) + std::abs(obj.m_col - current_col);
                if (dist < closest) {
                    closest = dist;
                    target_row = obj.m_row;
                    target_col = obj.m_col;
                    enemy_spotted = true;
                }
            }
        }
        
        if (enemy_spotted) {
            std::cout << "  Grenadier: Enemy at (" << target_row << "," << target_col 
                      << ") Grenades left: " << get_grenades() << std::endl;
        }
    }
    
    virtual bool get_shot_location(int& shot_row, int& shot_col) override
    {
        if (enemy_spotted && get_grenades() > 0) {
            shot_row = target_row;
            shot_col = target_col;
            std::cout << "  GRENADE at enemy position! 3x3 explosion!" << std::endl;
            return true;
        }
        return false;
    }
    
    virtual void get_move_direction(int& move_direction, int& move_distance) override
    {
        int current_row, current_col;
        get_current_location(current_row, current_col);
        
        if (enemy_spotted) {
            // Move toward enemy
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
        
        // Random search
        move_direction = (std::rand() % 8) + 1;
        move_distance = 1;
    }
};

extern "C" RobotBase* create_robot()
{
    return new Robot_Grenadier();
}

extern "C" const char* robot_summary()
{
    return "Grenadier: seeks enemies, area explosions.";
}
