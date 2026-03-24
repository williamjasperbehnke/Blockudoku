#include "blockudoku/gameplay_screen_renderer.h"

#include "bn_string.h"

#include "blockudoku/game_state.h"
#include "blockudoku/piece_library.h"
#include "blockudoku/ui_render_constants.h"
#include "blockudoku/ui_renderer.h"

namespace blockudoku
{
    void gameplay_screen_renderer::draw_header(ui_renderer& renderer, const game_state& state)
    {
        bn::string<20> score_text("SCORE ");
        score_text += bn::to_string<12>(state.score());
        renderer._text_generator.generate(38, -68, score_text, renderer._text_sprites);

        bn::string<20> moves_text("MOVES ");
        moves_text += bn::to_string<12>(state.moves_available());
        renderer._text_generator.generate(38, -56, moves_text, renderer._text_sprites);
    }

    void gameplay_screen_renderer::draw_board(ui_renderer& renderer, const game_state& state)
    {
        bool green_highlight[game_state::board_size][game_state::board_size] = {};

        const auto tile16_base = [&](int tile_index) -> int
        {
            switch(tile_index)
            {
                case ui_render_constants::tile_tan:
                    return ui_render_constants::tile16_tan_base;
                case ui_render_constants::tile_blue:
                    return ui_render_constants::tile16_blue_base;
                case ui_render_constants::tile_red:
                    return ui_render_constants::tile16_red_base;
                case ui_render_constants::tile_green:
                    return ui_render_constants::tile16_green_base;
                default:
                    return -1;
            }
        };

        const auto draw_scaled_board_cell = [&](int board_x, int board_y, int tile_index)
        {
            const int map_base_x =
                    ui_render_constants::board_bg_left + (board_x * ui_render_constants::board_cell_tiles);
            const int map_base_y =
                    ui_render_constants::board_bg_top + (board_y * ui_render_constants::board_cell_tiles);
            const int tile16 = tile16_base(tile_index);

            for(int cell_y = 0; cell_y < ui_render_constants::board_cell_tiles; ++cell_y)
            {
                for(int cell_x = 0; cell_x < ui_render_constants::board_cell_tiles; ++cell_x)
                {
                    int final_tile = ui_render_constants::tile_empty;
                    if(tile16 >= 0)
                    {
                        final_tile = tile16 + (cell_y * 2) + cell_x;
                    }
                    renderer.set_ui_cell(map_base_x + cell_x, map_base_y + cell_y, final_tile);
                }
            }
        };

        if(state.slot_active(state.selected_slot()) && state.can_place_selected_at_cursor())
        {
            bool occupied[game_state::board_size][game_state::board_size] = {};

            for(int y = 0; y < game_state::board_size; ++y)
            {
                for(int x = 0; x < game_state::board_size; ++x)
                {
                    occupied[y][x] = state.board_cell(x, y);
                }
            }

            const piece_def& piece = state.selected_piece();

            for(int y = 0; y < piece.height; ++y)
            {
                for(int x = 0; x < piece.width; ++x)
                {
                    if(piece_library::cell(piece, x, y))
                    {
                        occupied[state.cursor_y() + y][state.cursor_x() + x] = true;
                    }
                }
            }

            bool row_full[game_state::board_size] = {};
            bool col_full[game_state::board_size] = {};
            bool box_full[3][3] = {};

            for(int y = 0; y < game_state::board_size; ++y)
            {
                bool full = true;

                for(int x = 0; x < game_state::board_size; ++x)
                {
                    if(! occupied[y][x])
                    {
                        full = false;
                        break;
                    }
                }

                row_full[y] = full;
            }

            for(int x = 0; x < game_state::board_size; ++x)
            {
                bool full = true;

                for(int y = 0; y < game_state::board_size; ++y)
                {
                    if(! occupied[y][x])
                    {
                        full = false;
                        break;
                    }
                }

                col_full[x] = full;
            }

            for(int by = 0; by < 3; ++by)
            {
                for(int bx = 0; bx < 3; ++bx)
                {
                    bool full = true;

                    for(int y = 0; y < 3 && full; ++y)
                    {
                        for(int x = 0; x < 3; ++x)
                        {
                            if(! occupied[by * 3 + y][bx * 3 + x])
                            {
                                full = false;
                                break;
                            }
                        }
                    }

                    box_full[by][bx] = full;
                }
            }

            for(int y = 0; y < game_state::board_size; ++y)
            {
                for(int x = 0; x < game_state::board_size; ++x)
                {
                    if(occupied[y][x] && (row_full[y] || col_full[x] || box_full[y / 3][x / 3]))
                    {
                        green_highlight[y][x] = true;
                    }
                }
            }
        }

        for(int y = 0; y < game_state::board_size; ++y)
        {
            for(int x = 0; x < game_state::board_size; ++x)
            {
                int board_tile = ui_render_constants::tile_empty;

                if(state.board_cell(x, y))
                {
                    board_tile =
                            green_highlight[y][x] ? ui_render_constants::tile_green : ui_render_constants::tile_tan;
                }
                draw_scaled_board_cell(x, y, board_tile);
            }
        }

        if(state.slot_active(state.selected_slot()))
        {
            const piece_def& piece = state.selected_piece();
            const bool valid = state.can_place_selected_at_cursor();

            for(int y = 0; y < piece.height; ++y)
            {
                for(int x = 0; x < piece.width; ++x)
                {
                    if(piece_library::cell(piece, x, y))
                    {
                        const int board_x = state.cursor_x() + x;
                        const int board_y = state.cursor_y() + y;

                        int preview_tile = ui_render_constants::tile_blue;

                        if(valid && green_highlight[board_y][board_x])
                        {
                            preview_tile = ui_render_constants::tile_green;
                        }
                        else if(! valid && state.board_cell(board_x, board_y))
                        {
                            preview_tile = ui_render_constants::tile_red;
                        }
                        draw_scaled_board_cell(board_x, board_y, preview_tile);
                    }
                }
            }
        }
    }

    void gameplay_screen_renderer::draw_tray(ui_renderer& renderer, const game_state& state)
    {
        for(int slot = 0; slot < game_state::slot_count; ++slot)
        {
            const piece_def& piece = piece_library::at(state.slot_piece_index(slot));
            const bool selected = slot == state.selected_slot();

            const int slot_bg_y = ui_render_constants::tray_bg_top + (slot * 5) - 2;
            const int preview_left = ui_render_constants::tray_bg_left + 1;

            for(int y = 0; y < 4; ++y)
            {
                for(int x = 0; x < 5; ++x)
                {
                    const int map_x = preview_left + x;
                    const int map_y = slot_bg_y + y;

                    const bool active = state.slot_active(slot);
                    const bool filled = active && x < piece.width && y < piece.height && piece_library::cell(piece, x, y);
                    int tile_index = ui_render_constants::tile_empty;

                    if(filled)
                    {
                        if(! state.slot_can_place(slot))
                        {
                            tile_index = ui_render_constants::tile_red;
                        }
                        else if(selected)
                        {
                            tile_index = ui_render_constants::tile_blue;
                        }
                        else
                        {
                            tile_index = ui_render_constants::tile_tan;
                        }
                    }

                    renderer.set_ui_cell(map_x, map_y, tile_index);
                }
            }

            bn::string<4> label;
            label += bn::to_string<2>(slot + 1);
            if(selected)
            {
                renderer._selected_tray_generator.generate(92, -24 + (slot * 40), label, renderer._text_sprites);
            }
            else
            {
                renderer._text_generator.generate(92, -24 + (slot * 40), label, renderer._text_sprites);
            }
        }
    }

    void gameplay_screen_renderer::render(ui_renderer& renderer, const game_state& state)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::gameplay);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        draw_header(renderer, state);
        draw_board(renderer, state);
        draw_tray(renderer, state);
        renderer._feedback.update_careful_condition(state);
        renderer._feedback.draw_messages(
                state, renderer._text_generator, renderer._selected_tray_generator, renderer._text_sprites);
        renderer._feedback.apply_shake(renderer._gameplay_bg, renderer._ui_bg, renderer._text_sprites);

        renderer.commit_frame();
    }
}
