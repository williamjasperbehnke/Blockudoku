#ifndef BLOCKUDOKU_GAME_STATE_H
#define BLOCKUDOKU_GAME_STATE_H

#include "bn_random.h"

#include "blockudoku/game_event.h"
#include "blockudoku/piece_library.h"

namespace blockudoku
{

class game_state
{
public:
    static constexpr int board_size = 9;
    static constexpr int slot_count = 3;

    game_state();

    void reset();

    [[nodiscard]] bool move_cursor(int dx, int dy);

    [[nodiscard]] bool cycle_slot(int delta);

    [[nodiscard]] game_event try_place_selected();

    [[nodiscard]] bool game_over() const;

    [[nodiscard]] int score() const;

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
    [[nodiscard]] bool cycle_hint_move();

    [[nodiscard]] int moves_available() const;

private:
    bool _board[board_size][board_size] = {};
    int _slots[slot_count] = {};
    bool _slot_active[slot_count] = {};
    int _cursor_x = 0;
    int _cursor_y = 0;
    int _selected_slot = 0;
    int _score = 0;
    bool _game_over = false;
    bn::random _random;

    [[nodiscard]] int random_piece_index();

    [[nodiscard]] bool can_place(const piece_def& piece, int base_x, int base_y) const;

    void place_piece(const piece_def& piece, int base_x, int base_y);

    [[nodiscard]] int clear_completed_lines_and_boxes();

    [[nodiscard]] int slot_moves_available(int slot_index) const;

    [[nodiscard]] bool has_any_move() const;

    void clamp_cursor_to_selected_piece();

    void refill_slots();

    [[nodiscard]] bool all_slots_used() const;

    void select_first_active_slot();
};

}

#endif
