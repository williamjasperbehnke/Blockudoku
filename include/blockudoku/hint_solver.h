#ifndef BLOCKUDOKU_HINT_SOLVER_H
#define BLOCKUDOKU_HINT_SOLVER_H

namespace blockudoku
{

class game_state;

struct hint_move
{
    bool valid = false;
    int slot_index = 0;
    int base_x = 0;
    int base_y = 0;
    int heuristic = -1;
    int cleared_cells = 0;
    int remaining_moves = 0;
};

class hint_solver
{
public:
    [[nodiscard]] static hint_move find_best_move(const game_state& state);
};

}

#endif
