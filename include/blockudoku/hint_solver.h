#ifndef BLOCKUDOKU_HINT_SOLVER_H
#define BLOCKUDOKU_HINT_SOLVER_H

#include "blockudoku/game_state.h"

namespace blockudoku
{

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

class hint_search_task
{
public:
    void begin(const game_state& state);
    void cancel();
    [[nodiscard]] bool active() const;
    [[nodiscard]] bool finished() const;
    void step(int candidate_budget);
    [[nodiscard]] const hint_move& best_move() const;

private:
    bool _board[game_state::board_size][game_state::board_size] = {};
    int _slots[game_state::slot_count] = {};
    bool _slot_active[game_state::slot_count] = {};
    int _cursor_x = 0;
    int _cursor_y = 0;
    int _slot_index = 0;
    int _scan_x = 0;
    int _scan_y = 0;
    int _best_distance = 0;
    bool _active = false;
    bool _finished = false;
    hint_move _best_move;
};

}

#endif
