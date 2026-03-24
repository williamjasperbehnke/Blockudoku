#ifndef BLOCKUDOKU_PIECE_LIBRARY_H
#define BLOCKUDOKU_PIECE_LIBRARY_H

#include <cstdint>

namespace blockudoku
{

struct piece_def
{
    const char* name;
    int width;
    int height;
    uint32_t bits;
    int cell_count;
};

class piece_library
{
public:
    [[nodiscard]] static int count();

    [[nodiscard]] static const piece_def& at(int index);

    [[nodiscard]] static bool cell(const piece_def& piece, int x, int y);
};

}

#endif
