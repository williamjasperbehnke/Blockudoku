#include "blockudoku/board_rules.h"

namespace blockudoku::board_rules
{
    bool can_place(const bool (&board)[board_size][board_size], const piece_def& piece, int base_x, int base_y)
    {
        if(base_x < 0 || base_y < 0 || base_x + piece.width > board_size || base_y + piece.height > board_size)
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

    void place_piece(bool (&board)[board_size][board_size], const piece_def& piece, int base_x, int base_y)
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

    int clear_completed_lines_and_boxes(bool (&board)[board_size][board_size])
    {
        bool row_full[board_size] = {};
        bool column_full[board_size] = {};
        bool box_full[3][3] = {};

        for(int row = 0; row < board_size; ++row)
        {
            bool full = true;

            for(int column = 0; column < board_size; ++column)
            {
                if(! board[row][column])
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

        for(int row = 0; row < board_size; ++row)
        {
            for(int column = 0; column < board_size; ++column)
            {
                if(board[row][column] &&
                   (row_full[row] || column_full[column] || box_full[row / 3][column / 3]))
                {
                    board[row][column] = false;
                    ++cleared_count;
                }
            }
        }

        return cleared_count;
    }

    int slot_moves_available(
            const bool (&board)[board_size][board_size], const int (&slots)[slot_count],
            const bool (&slot_active)[slot_count], int slot_index)
    {
        if(! slot_active[slot_index])
        {
            return 0;
        }

        const piece_def& piece = piece_library::at(slots[slot_index]);
        int result = 0;

        for(int y = 0; y <= board_size - piece.height; ++y)
        {
            for(int x = 0; x <= board_size - piece.width; ++x)
            {
                if(can_place(board, piece, x, y))
                {
                    ++result;
                }
            }
        }

        return result;
    }

    bool has_any_move(
            const bool (&board)[board_size][board_size], const int (&slots)[slot_count],
            const bool (&slot_active)[slot_count])
    {
        for(int slot = 0; slot < slot_count; ++slot)
        {
            if(slot_moves_available(board, slots, slot_active, slot) > 0)
            {
                return true;
            }
        }

        return false;
    }
}
