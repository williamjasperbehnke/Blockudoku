#include "blockudoku/piece_library.h"

#include "bn_assert.h"

namespace blockudoku
{
    namespace
    {
        constexpr piece_def pieces[] = {
            { "1", 1, 1, 0b1, 1 },
            { "2H", 2, 1, 0b11, 2 },
            { "2V", 1, 2, 0b11, 2 },
            { "3H", 3, 1, 0b111, 3 },
            { "3V", 1, 3, 0b111, 3 },
            { "SQ2", 2, 2, 0b1111, 4 },
            { "L3A", 2, 2, 0b1101, 3 },
            { "L3B", 2, 2, 0b1110, 3 },
            { "L4", 2, 3, 0b111101, 4 },
            { "T4", 3, 2, 0b10111, 4 },
            { "P5", 3, 2, 0b111011, 5 },
            { "BAR5", 5, 1, 0b11111, 5 },
        };

        constexpr int pieces_count = int(sizeof(pieces) / sizeof(pieces[0]));
    }

    int piece_library::count()
    {
        return pieces_count;
    }

    const piece_def& piece_library::at(int index)
    {
        BN_ASSERT(index >= 0 && index < pieces_count, "Invalid piece index: ", index);
        return pieces[index];
    }

    bool piece_library::cell(const piece_def& piece, int x, int y)
    {
        BN_ASSERT(x >= 0 && y >= 0 && x < piece.width && y < piece.height, "Invalid piece cell");
        return (piece.bits >> (y * piece.width + x)) & 1;
    }
}
