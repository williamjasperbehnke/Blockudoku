#include "blockudoku/game_state.h"

#include "bn_algorithm.h"
#include "bn_array.h"
#include "bn_assert.h"

#include "blockudoku/board_rules.h"
#include "blockudoku/hint_solver.h"

namespace blockudoku
{
    game_state::game_state()
    {
        reset();
    }

    void game_state::set_run_seed(unsigned run_seed)
    {
        if(run_seed == 0)
        {
            run_seed = 1;
        }

        _run_seed = run_seed;
        _rng_state = run_seed;
    }

    unsigned game_state::run_seed() const
    {
        return _run_seed;
    }

    void game_state::reset()
    {
        if(_rng_state == 0)
        {
            _rng_state = _run_seed == 0 ? 1 : _run_seed;
        }

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
        _combo_streak = 0;
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

        if(! board_rules::can_place(_board, piece, _cursor_x, _cursor_y))
        {
            return { game_event_type::invalid, 0 };
        }

        board_rules::place_piece(_board, piece, _cursor_x, _cursor_y);
        const int cleared_cells = board_rules::clear_completed_lines_and_boxes(_board);
        const bool full_board_clear = cleared_cells > 0 && is_board_empty();
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

        int combo_bonus = 0;
        if(cleared_cells > 0)
        {
            ++_combo_streak;
            if(_combo_streak > 1)
            {
                combo_bonus = cleared_cells * _combo_streak * 2;
            }
        }
        else
        {
            _combo_streak = 0;
        }

        _score += piece.cell_count + (cleared_cells * 5) + combo_bonus +
                  (full_board_clear ? full_board_clear_bonus : 0);

        if(! has_any_move())
        {
            _game_over = true;
            return { game_event_type::game_over, cleared_cells, full_board_clear };
        }

        if(cleared_cells > 0)
        {
            return { game_event_type::cleared, cleared_cells, full_board_clear };
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

    int game_state::combo_streak() const
    {
        return _combo_streak;
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
        return board_rules::can_place(_board, selected_piece(), _cursor_x, _cursor_y);
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

    bool game_state::apply_hint_move(int slot_index, int base_x, int base_y)
    {
        if(_game_over || ! has_any_move())
        {
            return false;
        }

        if(slot_index < 0 || slot_index >= slot_count)
        {
            return false;
        }

        if(! slot_can_place(slot_index))
        {
            return false;
        }

        const piece_def& piece = piece_library::at(_slots[slot_index]);
        if(! board_rules::can_place(_board, piece, base_x, base_y))
        {
            return false;
        }

        _selected_slot = slot_index;
        _cursor_x = base_x;
        _cursor_y = base_y;
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
                    if(board_rules::can_place(_board, piece, x, y))
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

    void game_state::dev_refresh_tray()
    {
        refill_slots();
        _selected_slot = 0;
        clamp_cursor_to_selected_piece();
        _game_over = ! has_any_move();
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

    game_state::snapshot game_state::make_snapshot() const
    {
        snapshot state_snapshot;

        for(int y = 0; y < board_size; ++y)
        {
            for(int x = 0; x < board_size; ++x)
            {
                state_snapshot.board[y][x] = _board[y][x];
            }
        }

        for(int slot = 0; slot < slot_count; ++slot)
        {
            state_snapshot.slots[slot] = _slots[slot];
            state_snapshot.slot_active[slot] = _slot_active[slot];
        }

        state_snapshot.cursor_x = _cursor_x;
        state_snapshot.cursor_y = _cursor_y;
        state_snapshot.selected_slot = _selected_slot;
        state_snapshot.score = _score;
        state_snapshot.combo_streak = _combo_streak;
        state_snapshot.game_over = _game_over;
        state_snapshot.run_seed = _run_seed;
        state_snapshot.rng_state = _rng_state;
        return state_snapshot;
    }

    bool game_state::restore_snapshot(const snapshot& state_snapshot)
    {
        if(state_snapshot.selected_slot < 0 || state_snapshot.selected_slot >= slot_count)
        {
            return false;
        }

        if(state_snapshot.cursor_x < 0 || state_snapshot.cursor_x >= board_size ||
           state_snapshot.cursor_y < 0 || state_snapshot.cursor_y >= board_size)
        {
            return false;
        }

        if(state_snapshot.score < 0 || state_snapshot.combo_streak < 0)
        {
            return false;
        }

        for(int slot = 0; slot < slot_count; ++slot)
        {
            if(state_snapshot.slots[slot] < 0 || state_snapshot.slots[slot] >= piece_library::count())
            {
                return false;
            }
        }

        for(int y = 0; y < board_size; ++y)
        {
            for(int x = 0; x < board_size; ++x)
            {
                _board[y][x] = state_snapshot.board[y][x];
            }
        }

        for(int slot = 0; slot < slot_count; ++slot)
        {
            _slots[slot] = state_snapshot.slots[slot];
            _slot_active[slot] = state_snapshot.slot_active[slot];
        }

        _cursor_x = state_snapshot.cursor_x;
        _cursor_y = state_snapshot.cursor_y;
        _selected_slot = state_snapshot.selected_slot;
        _score = state_snapshot.score;
        _combo_streak = state_snapshot.combo_streak;
        _game_over = state_snapshot.game_over;
        _run_seed = state_snapshot.run_seed == 0 ? 1 : state_snapshot.run_seed;
        _rng_state = state_snapshot.rng_state == 0 ? _run_seed : state_snapshot.rng_state;

        bool any_active_slot = false;
        for(bool slot_active : _slot_active)
        {
            if(slot_active)
            {
                any_active_slot = true;
                break;
            }
        }

        if(! any_active_slot)
        {
            refill_slots();
            _selected_slot = 0;
        }
        else if(! _slot_active[_selected_slot])
        {
            select_first_active_slot();
        }

        clamp_cursor_to_selected_piece();

        if(! has_any_move())
        {
            _game_over = true;
        }

        return true;
    }

    int game_state::random_piece_index()
    {
        return int(next_random_value() % unsigned(piece_library::count()));
    }

    unsigned game_state::next_random_value()
    {
        // Xorshift32; deterministic from the run seed.
        unsigned value = _rng_state;
        value ^= value << 13;
        value ^= value >> 17;
        value ^= value << 5;
        _rng_state = value == 0 ? 0xA341316Cu : value;
        return _rng_state;
    }

    int game_state::slot_moves_available(int slot_index) const
    {
        return board_rules::slot_moves_available(_board, _slots, _slot_active, slot_index);
    }

    bool game_state::has_any_move() const
    {
        return board_rules::has_any_move(_board, _slots, _slot_active);
    }

    bool game_state::is_board_empty() const
    {
        for(int y = 0; y < board_size; ++y)
        {
            for(int x = 0; x < board_size; ++x)
            {
                if(_board[y][x])
                {
                    return false;
                }
            }
        }

        return true;
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
