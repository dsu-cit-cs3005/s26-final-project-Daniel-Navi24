#include "RobotBase.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

class Robot_Ratboy : public RobotBase
{
private:
    bool m_moving_down = true;
    int to_shoot_row = -1;
    int to_shoot_col = -1;
    std::vector<RadarObj> known_obstacles;

    bool is_obstacle(int row, int col) const
    {
        return std::any_of(known_obstacles.begin(), known_obstacles.end(),
                           [&](const RadarObj& obj) {
                               return obj.m_row == row && obj.m_col == col;
                           });
    }

    void clear_target()
    {
        to_shoot_row = -1;
        to_shoot_col = -1;
    }

    void add_obstacle(const RadarObj& obj)
    {
        if ((obj.m_type == 'M' || obj.m_type == 'P' || obj.m_type == 'F') &&
            !is_obstacle(obj.m_row, obj.m_col))
        {
            known_obstacles.push_back(obj);
        }
    }

public:
    Robot_Ratboy() : RobotBase(3, 4, railgun)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    virtual void get_radar_direction(int& radar_direction) override
    {
        int current_row, current_col;
        get_current_location(current_row, current_col);

        if (to_shoot_row != -1 && to_shoot_col != -1) {
            int row_diff = to_shoot_row - current_row;
            int col_diff = to_shoot_col - current_col;
            
            if (row_diff < 0 && col_diff == 0) radar_direction = 1;
            else if (row_diff < 0 && col_diff > 0) radar_direction = 2;
            else if (row_diff == 0 && col_diff > 0) radar_direction = 3;
            else if (row_diff > 0 && col_diff > 0) radar_direction = 4;
            else if (row_diff > 0 && col_diff == 0) radar_direction = 5;
            else if (row_diff > 0 && col_diff < 0) radar_direction = 6;
            else if (row_diff == 0 && col_diff < 0) radar_direction = 7;
            else if (row_diff < 0 && col_diff < 0) radar_direction = 8;
            else radar_direction = 0;
            return;
        }
        radar_direction = 0;
    }

    virtual void process_radar_results(const std::vector<RadarObj>& radar_results) override
    {
        clear_target();

        for (const auto& obj : radar_results) {
            add_obstacle(obj);

            if (obj.m_type == 'R' && to_shoot_row == -1 && to_shoot_col == -1) {
                to_shoot_row = obj.m_row;
                to_shoot_col = obj.m_col;
                std::cout << "  Ratboy: Target at (" << to_shoot_row << "," << to_shoot_col << ")" << std::endl;
            }
        }
    }

    virtual bool get_shot_location(int& shot_row, int& shot_col) override
    {
        if (to_shoot_row != -1 && to_shoot_col != -1) {
            shot_row = to_shoot_row;
            shot_col = to_shoot_col;
            return true;
        }
        return false;
    }

    virtual void get_move_direction(int& move_direction, int& move_distance) override
    {
        int current_row, current_col;
        get_current_location(current_row, current_col);
        int max_move = get_move_speed();
        
        if (to_shoot_row != -1 && to_shoot_col != -1) {
            int row_diff = to_shoot_row - current_row;
            int col_diff = to_shoot_col - current_col;
            
            if (std::abs(row_diff) > std::abs(col_diff)) {
                move_direction = (row_diff > 0) ? 5 : 1;
            } else if (col_diff != 0) {
                move_direction = (col_diff > 0) ? 3 : 7;
            } else {
                move_direction = 0;
            }
            move_distance = 1;
            return;
        }
        
        for (int attempt = 0; attempt < 3; attempt++) {
            move_direction = (std::rand() % 8) + 1;
            int dr = directions[move_direction].first;
            int dc = directions[move_direction].second;
            int new_row = current_row + dr;
            int new_col = current_col + dc;
            
            if (new_row >= 0 && new_row < m_board_row_max &&
                new_col >= 0 && new_col < m_board_col_max) {
                move_distance = 1;
                return;
            }
        }
        
        move_direction = 0;
        move_distance = 0;
    }
};

extern "C" RobotBase* create_robot()
{
    return new Robot_Ratboy();
}

extern "C" const char* robot_summary()
{
    return "Hugs left wall, railguns nearest target.";
}
