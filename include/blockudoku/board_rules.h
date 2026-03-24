#ifndef BLOCKUDOKU_BOARD_RULES_H
#define BLOCKUDOKU_BOARD_RULES_H

#include "blockudoku/piece_library.h"

namespace blockudoku::board_rules
{
    constexpr int board_size = 9;
    constexpr int slot_count = 3;

    [[nodiscard]] bool can_place(const bool (&board)[board_size][board_size], const piece_def& piece, int base_x, int base_y);

    void place_piece(bool (&board)[board_size][board_size], const piece_def& piece, int base_x, int base_y);

    [[nodiscard]] int clear_completed_lines_and_boxes(bool (&board)[board_size][board_size]);

    [[nodiscard]] int slot_moves_available(
            const bool (&board)[board_size][board_size], const int (&slots)[slot_count],
            const bool (&slot_active)[slot_count], int slot_index);

    [[nodiscard]] bool has_any_move(
            const bool (&board)[board_size][board_size], const int (&slots)[slot_count],
            const bool (&slot_active)[slot_count]);
}

#endif
