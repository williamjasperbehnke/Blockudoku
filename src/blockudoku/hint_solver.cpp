#include "blockudoku/hint_solver.h"

#include "blockudoku/game_state.h"
#include "blockudoku/piece_library.h"

namespace blockudoku
{
    namespace
    {
        using board_snapshot = bool[game_state::board_size][game_state::board_size];

        bool can_place_on_board(const board_snapshot& board, const piece_def& piece, int base_x, int base_y)
        {
            if(base_x < 0 || base_y < 0 || base_x + piece.width > game_state::board_size ||
               base_y + piece.height > game_state::board_size)
            {
                return false;
            }

            for(int y = 0; y < piece.height; ++y)
            {
                for(int x = 0; x < piece.width; ++x)
                {
                    if(piece_library::cell(piece, x, y) && board[base_y + y][base_x + x])
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        void place_piece_on_board(board_snapshot& board, const piece_def& piece, int base_x, int base_y)
        {
            for(int y = 0; y < piece.height; ++y)
            {
                for(int x = 0; x < piece.width; ++x)
                {
                    if(piece_library::cell(piece, x, y))
                    {
                        board[base_y + y][base_x + x] = true;
                    }
                }
            }
        }

        int clear_completed_lines_and_boxes(board_snapshot& board)
        {
            bool row_full[game_state::board_size] = {};
            bool column_full[game_state::board_size] = {};
            bool box_full[3][3] = {};

            for(int row = 0; row < game_state::board_size; ++row)
            {
                bool full = true;

                for(int column = 0; column < game_state::board_size; ++column)
                {
                    if(! board[row][column])
                    {
                        full = false;
                        break;
                    }
                }

                row_full[row] = full;
            }

            for(int column = 0; column < game_state::board_size; ++column)
            {
                bool full = true;

                for(int row = 0; row < game_state::board_size; ++row)
                {
                    if(! board[row][column])
                    {
                        full = false;
                        break;
                    }
                }

                column_full[column] = full;
            }

            for(int box_y = 0; box_y < 3; ++box_y)
            {
                for(int box_x = 0; box_x < 3; ++box_x)
                {
                    bool full = true;

                    for(int local_y = 0; local_y < 3 && full; ++local_y)
                    {
                        for(int local_x = 0; local_x < 3; ++local_x)
                        {
                            if(! board[box_y * 3 + local_y][box_x * 3 + local_x])
                            {
                                full = false;
                                break;
                            }
                        }
                    }

                    box_full[box_y][box_x] = full;
                }
            }

            int cleared_count = 0;

            for(int row = 0; row < game_state::board_size; ++row)
            {
                for(int column = 0; column < game_state::board_size; ++column)
                {
                    if(board[row][column] && (row_full[row] || column_full[column] || box_full[row / 3][column / 3]))
                    {
                        board[row][column] = false;
                        ++cleared_count;
                    }
                }
            }

            return cleared_count;
        }

        int count_moves_for_piece(const board_snapshot& board, const piece_def& piece)
        {
            int move_count = 0;

            for(int y = 0; y <= game_state::board_size - piece.height; ++y)
            {
                for(int x = 0; x <= game_state::board_size - piece.width; ++x)
                {
                    if(can_place_on_board(board, piece, x, y))
                    {
                        ++move_count;
                    }
                }
            }

            return move_count;
        }

        int score_move(const piece_def& piece, int cleared_cells, int remaining_moves)
        {
            const int placement_score = piece.cell_count;
            const int clear_score = cleared_cells * 5;
            const int clear_priority = cleared_cells * 20;
            const int mobility_score = remaining_moves * 2;
            return placement_score + clear_score + clear_priority + mobility_score;
        }
    }

    hint_move hint_solver::find_best_move(const game_state& state)
    {
        hint_search_task task;
        task.begin(state);

        while(task.active())
        {
            task.step(1024);
        }

        return task.best_move();
    }

    void hint_search_task::begin(const game_state& state)
    {
        for(int y = 0; y < game_state::board_size; ++y)
        {
            for(int x = 0; x < game_state::board_size; ++x)
            {
                _board[y][x] = state.board_cell(x, y);
            }
        }

        for(int slot = 0; slot < game_state::slot_count; ++slot)
        {
            _slots[slot] = state.slot_piece_index(slot);
            _slot_active[slot] = state.slot_active(slot);
        }

        _cursor_x = state.cursor_x();
        _cursor_y = state.cursor_y();
        _slot_index = 0;
        _scan_x = 0;
        _scan_y = 0;
        _best_distance = 0;
        _best_move = {};
        _active = true;
        _finished = false;
    }

    void hint_search_task::cancel()
    {
        _active = false;
        _finished = false;
        _best_move = {};
    }

    bool hint_search_task::active() const
    {
        return _active;
    }

    bool hint_search_task::finished() const
    {
        return _finished;
    }

    void hint_search_task::step(int candidate_budget)
    {
        if(! _active || _finished || candidate_budget <= 0)
        {
            return;
        }

        while(candidate_budget > 0 && _slot_index < game_state::slot_count)
        {
            if(! _slot_active[_slot_index])
            {
                ++_slot_index;
                _scan_x = 0;
                _scan_y = 0;
                continue;
            }

            const piece_def& piece = piece_library::at(_slots[_slot_index]);
            const int max_x = game_state::board_size - piece.width;
            const int max_y = game_state::board_size - piece.height;

            while(_scan_y <= max_y)
            {
                while(_scan_x <= max_x)
                {
                    const int test_x = _scan_x;
                    const int test_y = _scan_y;
                    ++_scan_x;

                    if(! can_place_on_board(_board, piece, test_x, test_y))
                    {
                        continue;
                    }

                    board_snapshot simulated_board = {};

                    for(int row = 0; row < game_state::board_size; ++row)
                    {
                        for(int column = 0; column < game_state::board_size; ++column)
                        {
                            simulated_board[row][column] = _board[row][column];
                        }
                    }

                    place_piece_on_board(simulated_board, piece, test_x, test_y);
                    const int cleared_cells = clear_completed_lines_and_boxes(simulated_board);

                    int remaining_moves = 0;
                    for(int other_slot = 0; other_slot < game_state::slot_count; ++other_slot)
                    {
                        if(other_slot == _slot_index || ! _slot_active[other_slot])
                        {
                            continue;
                        }

                        const piece_def& other_piece = piece_library::at(_slots[other_slot]);
                        remaining_moves += count_moves_for_piece(simulated_board, other_piece);
                    }

                    const int heuristic = score_move(piece, cleared_cells, remaining_moves);
                    const int distance = (test_x > _cursor_x ? test_x - _cursor_x : _cursor_x - test_x) +
                                         (test_y > _cursor_y ? test_y - _cursor_y : _cursor_y - test_y);

                    const bool better_score = ! _best_move.valid || heuristic > _best_move.heuristic;
                    const bool equal_score_better_clear =
                            _best_move.valid && heuristic == _best_move.heuristic && cleared_cells > _best_move.cleared_cells;
                    const bool equal_score_equal_clear_better_mobility =
                            _best_move.valid && heuristic == _best_move.heuristic &&
                            cleared_cells == _best_move.cleared_cells && remaining_moves > _best_move.remaining_moves;
                    const bool tie_break_closer =
                            _best_move.valid && heuristic == _best_move.heuristic &&
                            cleared_cells == _best_move.cleared_cells &&
                            remaining_moves == _best_move.remaining_moves && distance < _best_distance;

                    if(better_score || equal_score_better_clear || equal_score_equal_clear_better_mobility || tie_break_closer)
                    {
                        _best_move.valid = true;
                        _best_move.slot_index = _slot_index;
                        _best_move.base_x = test_x;
                        _best_move.base_y = test_y;
                        _best_move.heuristic = heuristic;
                        _best_move.cleared_cells = cleared_cells;
                        _best_move.remaining_moves = remaining_moves;
                        _best_distance = distance;
                    }

                    --candidate_budget;
                    if(candidate_budget <= 0)
                    {
                        return;
                    }
                }

                _scan_x = 0;
                ++_scan_y;
            }

            ++_slot_index;
            _scan_x = 0;
            _scan_y = 0;
        }

        if(_slot_index >= game_state::slot_count)
        {
            _active = false;
            _finished = true;
        }
    }

    const hint_move& hint_search_task::best_move() const
    {
        return _best_move;
    }
}
