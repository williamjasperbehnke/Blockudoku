#ifndef BLOCKUDOKU_UI_RENDER_CONSTANTS_H
#define BLOCKUDOKU_UI_RENDER_CONSTANTS_H

namespace blockudoku::ui_render_constants
{
    constexpr int seed_digits_count = 8;
    constexpr int board_bg_left = 2;
    constexpr int board_bg_top = 7;
    constexpr int board_cell_tiles = 2;

    constexpr int tray_bg_left = 21;
    constexpr int tray_bg_top = 13;

    constexpr int tile_empty = 0;
    constexpr int tile_tan = 1;
    constexpr int tile_blue = 2;
    constexpr int tile_red = 3;
    constexpr int tile_green = 4;

    constexpr int tile16_tan_base = 5;
    constexpr int tile16_blue_base = 9;
    constexpr int tile16_red_base = 13;
    constexpr int tile16_green_base = 17;
}

#endif
