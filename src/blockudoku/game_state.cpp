#include "blockudoku/game_state.h"

#include "bn_algorithm.h"
#include "bn_array.h"
#include "bn_assert.h"

#include "blockudoku/hint_solver.h"

namespace blockudoku
{
    game_state::game_state()
    {
        reset();
    }

    void game_state::reset()
    {
        for(int y = 0; y < board_size; ++y)
        {
            for(int x = 0; x < board_size; ++x)
            {
                _board[y][x] = false;
            }
        }

        refill_slots();
        _cursor_x = 0;
        _cursor_y = 0;
        _selected_slot = 0;
        _score = 0;
        clamp_cursor_to_selected_piece();
        _game_over = ! has_any_move();
    }

    bool game_state::move_cursor(int dx, int dy)
    {
        if(_game_over)
        {
            return false;
        }

        if(! _slot_active[_selected_slot])
        {
            select_first_active_slot();
        }

        if(! slot_can_place(_selected_slot))
        {
            select_first_active_slot();
        }

        const piece_def& piece = selected_piece();
        const int max_x = board_size - piece.width;
        const int max_y = board_size - piece.height;
        const int new_x = bn::clamp(_cursor_x + dx, 0, max_x);
        const int new_y = bn::clamp(_cursor_y + dy, 0, max_y);
        const bool moved = new_x != _cursor_x || new_y != _cursor_y;

        _cursor_x = new_x;
        _cursor_y = new_y;
        return moved;
    }

    bool game_state::cycle_slot(int delta)
    {
        if(_game_over)
        {
            return false;
        }

        if(! has_any_move())
        {
            return false;
        }

        const int direction = delta >= 0 ? 1 : -1;
        int next = _selected_slot;

        for(int count = 0; count < slot_count; ++count)
        {
            next = (next + slot_count + direction) % slot_count;

            if(slot_can_place(next))
            {
                break;
            }
        }

        const bool changed = next != _selected_slot;
        _selected_slot = next;
        clamp_cursor_to_selected_piece();
        return changed;
    }

    game_event game_state::try_place_selected()
    {
        if(_game_over)
        {
            return { game_event_type::none, 0 };
        }

        if(! _slot_active[_selected_slot])
        {
            return { game_event_type::invalid, 0 };
        }

        const piece_def& piece = selected_piece();

        if(! can_place(piece, _cursor_x, _cursor_y))
        {
            return { game_event_type::invalid, 0 };
        }

        place_piece(piece, _cursor_x, _cursor_y);
        const int cleared_cells = clear_completed_lines_and_boxes();
        _slot_active[_selected_slot] = false;

        if(all_slots_used())
        {
            refill_slots();
        }
        else
        {
            select_first_active_slot();
        }

        clamp_cursor_to_selected_piece();

        _score += piece.cell_count + (cleared_cells * 5);

        if(! has_any_move())
        {
            _game_over = true;
            return { game_event_type::game_over, cleared_cells };
        }

        if(cleared_cells > 0)
        {
            return { game_event_type::cleared, cleared_cells };
        }

        return { game_event_type::placed, 0 };
    }

    bool game_state::game_over() const
    {
        return _game_over;
    }

    int game_state::score() const
    {
        return _score;
    }

    int game_state::cursor_x() const
    {
        return _cursor_x;
    }

    int game_state::cursor_y() const
    {
        return _cursor_y;
    }

    int game_state::selected_slot() const
    {
        return _selected_slot;
    }

    int game_state::slot_piece_index(int slot_index) const
    {
        BN_ASSERT(slot_index >= 0 && slot_index < slot_count, "Invalid slot index: ", slot_index);
        return _slots[slot_index];
    }

    bool game_state::slot_active(int slot_index) const
    {
        BN_ASSERT(slot_index >= 0 && slot_index < slot_count, "Invalid slot index: ", slot_index);
        return _slot_active[slot_index];
    }

    bool game_state::slot_can_place(int slot_index) const
    {
        BN_ASSERT(slot_index >= 0 && slot_index < slot_count, "Invalid slot index: ", slot_index);
        return _slot_active[slot_index] && slot_moves_available(slot_index) > 0;
    }

    const piece_def& game_state::selected_piece() const
    {
        BN_ASSERT(_slot_active[_selected_slot], "Selected slot inactive");
        return piece_library::at(_slots[_selected_slot]);
    }

    bool game_state::board_cell(int x, int y) const
    {
        BN_ASSERT(x >= 0 && x < board_size && y >= 0 && y < board_size, "Invalid board cell");
        return _board[y][x];
    }

    bool game_state::can_place_selected_at_cursor() const
    {
        return can_place(selected_piece(), _cursor_x, _cursor_y);
    }

    bool game_state::selected_piece_contains(int x, int y) const
    {
        const piece_def& piece = selected_piece();
        const int local_x = x - _cursor_x;
        const int local_y = y - _cursor_y;

        if(local_x < 0 || local_y < 0 || local_x >= piece.width || local_y >= piece.height)
        {
            return false;
        }

        return piece_library::cell(piece, local_x, local_y);
    }

    bool game_state::apply_hint()
    {
        if(_game_over || ! has_any_move())
        {
            return false;
        }

        const hint_move best_move = hint_solver::find_best_move(*this);
        if(! best_move.valid)
        {
            return false;
        }

        _selected_slot = best_move.slot_index;
        _cursor_x = best_move.base_x;
        _cursor_y = best_move.base_y;
        clamp_cursor_to_selected_piece();
        return true;
    }

    bool game_state::cycle_hint_move()
    {
        if(_game_over || ! has_any_move())
        {
            return false;
        }

        struct candidate_move
        {
            int slot = 0;
            int x = 0;
            int y = 0;
        };

        constexpr int max_candidates = slot_count * board_size * board_size;
        bn::array<candidate_move, max_candidates> candidates = {};
        int candidate_count = 0;

        for(int slot_offset = 0; slot_offset < slot_count; ++slot_offset)
        {
            const int slot = (_selected_slot + slot_offset) % slot_count;
            if(! slot_can_place(slot))
            {
                continue;
            }

            const piece_def& piece = piece_library::at(_slots[slot]);

            for(int y = 0; y <= board_size - piece.height; ++y)
            {
                for(int x = 0; x <= board_size - piece.width; ++x)
                {
                    if(can_place(piece, x, y))
                    {
                        candidates[candidate_count++] = { slot, x, y };
                    }
                }
            }
        }

        if(candidate_count == 0)
        {
            return false;
        }

        int current_index = -1;
        for(int index = 0; index < candidate_count; ++index)
        {
            const candidate_move& candidate = candidates[index];
            if(candidate.slot == _selected_slot && candidate.x == _cursor_x && candidate.y == _cursor_y)
            {
                current_index = index;
                break;
            }
        }

        const int next_index = current_index >= 0 ? (current_index + 1) % candidate_count : 0;
        const candidate_move& next_move = candidates[next_index];

        const bool changed = next_move.slot != _selected_slot || next_move.x != _cursor_x || next_move.y != _cursor_y;
        _selected_slot = next_move.slot;
        _cursor_x = next_move.x;
        _cursor_y = next_move.y;
        return changed;
    }

    int game_state::moves_available() const
    {
        int result = 0;

        for(int slot = 0; slot < slot_count; ++slot)
        {
            if(_slot_active[slot])
            {
                result += slot_moves_available(slot);
            }
        }

        return result;
    }

    int game_state::random_piece_index()
    {
        return _random.get_int(piece_library::count());
    }

    bool game_state::can_place(const piece_def& piece, int base_x, int base_y) const
    {
        if(base_x < 0 || base_y < 0 || base_x + piece.width > board_size || base_y + piece.height > board_size)
        {
            return false;
        }

        for(int y = 0; y < piece.height; ++y)
        {
            for(int x = 0; x < piece.width; ++x)
            {
                if(piece_library::cell(piece, x, y) && _board[base_y + y][base_x + x])
                {
                    return false;
                }
            }
        }

        return true;
    }

    void game_state::place_piece(const piece_def& piece, int base_x, int base_y)
    {
        for(int y = 0; y < piece.height; ++y)
        {
            for(int x = 0; x < piece.width; ++x)
            {
                if(piece_library::cell(piece, x, y))
                {
                    _board[base_y + y][base_x + x] = true;
                }
            }
        }
    }

    int game_state::clear_completed_lines_and_boxes()
    {
        bool row_full[board_size] = {};
        bool column_full[board_size] = {};
        bool box_full[3][3] = {};

        for(int row = 0; row < board_size; ++row)
        {
            bool full = true;

            for(int column = 0; column < board_size; ++column)
            {
                if(! _board[row][column])
                {
                    full = false;
                    break;
                }
            }

            row_full[row] = full;
        }

        for(int column = 0; column < board_size; ++column)
        {
            bool full = true;

            for(int row = 0; row < board_size; ++row)
            {
                if(! _board[row][column])
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
                        if(! _board[box_y * 3 + local_y][box_x * 3 + local_x])
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

        for(int row = 0; row < board_size; ++row)
        {
            for(int column = 0; column < board_size; ++column)
            {
                if(_board[row][column] &&
                   (row_full[row] || column_full[column] || box_full[row / 3][column / 3]))
                {
                    _board[row][column] = false;
                    ++cleared_count;
                }
            }
        }

        return cleared_count;
    }

    int game_state::slot_moves_available(int slot_index) const
    {
        const piece_def& piece = piece_library::at(_slots[slot_index]);
        int result = 0;

        if(! _slot_active[slot_index])
        {
            return 0;
        }

        for(int y = 0; y <= board_size - piece.height; ++y)
        {
            for(int x = 0; x <= board_size - piece.width; ++x)
            {
                if(can_place(piece, x, y))
                {
                    ++result;
                }
            }
        }

        return result;
    }

    bool game_state::has_any_move() const
    {
        for(int slot = 0; slot < slot_count; ++slot)
        {
            if(slot_moves_available(slot) > 0)
            {
                return true;
            }
        }

        return false;
    }

    void game_state::clamp_cursor_to_selected_piece()
    {
        if(all_slots_used())
        {
            _cursor_x = 0;
            _cursor_y = 0;
            return;
        }

        if(! _slot_active[_selected_slot])
        {
            select_first_active_slot();
        }

        const piece_def& piece = selected_piece();
        const int max_x = board_size - piece.width;
        const int max_y = board_size - piece.height;
        _cursor_x = bn::clamp(_cursor_x, 0, max_x);
        _cursor_y = bn::clamp(_cursor_y, 0, max_y);
    }

    void game_state::refill_slots()
    {
        for(int index = 0; index < slot_count; ++index)
        {
            _slots[index] = random_piece_index();
            _slot_active[index] = true;
        }
    }

    bool game_state::all_slots_used() const
    {
        for(int slot = 0; slot < slot_count; ++slot)
        {
            if(_slot_active[slot])
            {
                return false;
            }
        }

        return true;
    }

    void game_state::select_first_active_slot()
    {
        for(int slot = 0; slot < slot_count; ++slot)
        {
            if(slot_can_place(slot))
            {
                _selected_slot = slot;
                return;
            }
        }

        for(int slot = 0; slot < slot_count; ++slot)
        {
            if(_slot_active[slot])
            {
                _selected_slot = slot;
                return;
            }
        }
    }
}
