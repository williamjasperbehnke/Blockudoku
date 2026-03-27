#ifndef BLOCKUDOKU_GAME_STATE_H
#define BLOCKUDOKU_GAME_STATE_H

#include "blockudoku/game_event.h"
#include "blockudoku/piece_library.h"

namespace blockudoku
{

class game_state
{
public:
    static constexpr int board_size = 9;
    static constexpr int slot_count = 3;
    static constexpr int full_board_clear_bonus = 150;

    struct snapshot
    {
        bool board[board_size][board_size] = {};
        int slots[slot_count] = {};
        bool slot_active[slot_count] = {};
        int cursor_x = 0;
        int cursor_y = 0;
        int selected_slot = 0;
        int score = 0;
        int combo_streak = 0;
        bool game_over = false;
        unsigned run_seed = 1;
        unsigned rng_state = 1;
    };

    game_state();

    void set_run_seed(unsigned run_seed);
    [[nodiscard]] unsigned run_seed() const;

    void reset();

    [[nodiscard]] bool move_cursor(int dx, int dy);

    [[nodiscard]] bool cycle_slot(int delta);

    [[nodiscard]] game_event try_place_selected();

    [[nodiscard]] bool game_over() const;

    [[nodiscard]] int score() const;
    [[nodiscard]] int combo_streak() const;

    [[nodiscard]] int cursor_x() const;

    [[nodiscard]] int cursor_y() const;

    [[nodiscard]] int selected_slot() const;

    [[nodiscard]] int slot_piece_index(int slot_index) const;

    [[nodiscard]] bool slot_active(int slot_index) const;

    [[nodiscard]] bool slot_can_place(int slot_index) const;

    [[nodiscard]] const piece_def& selected_piece() const;

    [[nodiscard]] bool board_cell(int x, int y) const;

    [[nodiscard]] bool can_place_selected_at_cursor() const;

    [[nodiscard]] bool selected_piece_contains(int x, int y) const;

    [[nodiscard]] bool apply_hint();
    [[nodiscard]] bool apply_hint_move(int slot_index, int base_x, int base_y);
    [[nodiscard]] bool cycle_hint_move();
    void dev_refresh_tray();
    void dev_adjust_score(int delta);

    [[nodiscard]] int moves_available() const;
    [[nodiscard]] snapshot make_snapshot() const;
    [[nodiscard]] bool restore_snapshot(const snapshot& state_snapshot);

private:
    bool _board[board_size][board_size] = {};
    int _slots[slot_count] = {};
    bool _slot_active[slot_count] = {};
    int _cursor_x = 0;
    int _cursor_y = 0;
    int _selected_slot = 0;
    int _score = 0;
    int _combo_streak = 0;
    bool _game_over = false;
    unsigned _run_seed = 1;
    unsigned _rng_state = 1;

    [[nodiscard]] int random_piece_index();
    [[nodiscard]] unsigned next_random_value();
    [[nodiscard]] int slot_moves_available(int slot_index) const;
    [[nodiscard]] bool has_any_move() const;
    [[nodiscard]] bool is_board_empty() const;

    void clamp_cursor_to_selected_piece();

    void refill_slots();

    [[nodiscard]] bool all_slots_used() const;

    void select_first_active_slot();
};

}

#endif
