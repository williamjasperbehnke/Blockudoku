#include "blockudoku/ui_renderer.h"

#include "bn_array.h"
#include "bn_bpp_mode.h"
#include "bn_color.h"
#include "bn_memory.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_string.h"

#include "bn_bg_palette_items_palette_classic.h"
#include "bn_bg_palette_items_palette_mint.h"
#include "bn_bg_palette_items_palette_sunrise.h"
#include "bn_regular_bg_items_gameplay_bg.h"
#include "bn_regular_bg_items_info_bg.h"
#include "bn_regular_bg_items_title_bg.h"
#include "bn_regular_bg_tiles_items_block_type_classic.h"
#include "bn_regular_bg_tiles_items_block_type_facet.h"
#include "bn_regular_bg_tiles_items_block_type_strata.h"

#include "common_fixed_8x8_sprite_font.h"

#include "blockudoku/game_state.h"
#include "blockudoku/high_scores.h"
#include "blockudoku/piece_library.h"

namespace blockudoku
{
    namespace
    {
        constexpr int board_bg_left = 4;
        constexpr int board_bg_top = 11;

        constexpr int tray_bg_left = 21;
        constexpr int tray_bg_top = 11;

        constexpr int tile_empty = 0;
        constexpr int tile_tan = 1;
        constexpr int tile_blue = 2;
        constexpr int tile_red = 3;
        constexpr int tile_green = 4;
    }

    ui_renderer::ui_bg_map::ui_bg_map() :
        map_item(cells[0], bn::size(columns, rows))
    {
        bn::memory::clear(cells);
    }

    ui_renderer::ui_renderer() :
        _ui_bg_map(new ui_bg_map()),
        _text_generator(common::fixed_8x8_sprite_font),
        _accent_generator(common::fixed_8x8_sprite_font)
    {
        _text_generator.set_left_alignment();
        _accent_generator.set_left_alignment();
        build_static_bg();
    }

    void ui_renderer::set_last_event(const game_event& event)
    {
        _last_event = event;
    }

    void ui_renderer::set_block_style(int block_style)
    {
        if(block_style < 0)
        {
            _block_style = 0;
        }
        else
        {
            _block_style = block_style % block_style_count;
        }

        if(_block_style != _loaded_block_style)
        {
            rebuild_ui_bg();
        }
    }

    void ui_renderer::set_palette_style(int palette_style)
    {
        if(palette_style < 0)
        {
            _palette_style = 0;
        }
        else
        {
            _palette_style = palette_style % palette_style_count;
        }

        if(_palette_style != _loaded_palette_style)
        {
            rebuild_ui_bg();
        }
    }

    void ui_renderer::render(const game_state& state)
    {
        set_scene_background(scene_bg_type::gameplay);
        clear_ui_map();
        _text_sprites.clear();

        draw_header(state);
        draw_board(state);
        draw_tray(state);
        draw_controls(state);

        commit_frame();
    }

    void ui_renderer::render_main_menu(const high_scores& scores, int menu_index, int sfx_volume_percent, int music_volume_percent, int block_style, int palette_style)
    {
        set_scene_background(scene_bg_type::title);
        clear_ui_map();
        _text_sprites.clear();
        set_block_style(block_style);
        set_palette_style(palette_style);
        draw_title_blocks();

        _accent_generator.generate(-104, -68, "BLOCKUDOKU", _text_sprites);
        _text_generator.generate(-104, -52, "GBA EDITION", _text_sprites);

        _accent_generator.generate(-104, -28, "MENU", _text_sprites);
        _text_generator.generate(-104, -18, menu_index == 0 ? "> START GAME" : "  START GAME", _text_sprites);
        _text_generator.generate(-104, -8, menu_index == 1 ? "> HIGH SCORES" : "  HIGH SCORES", _text_sprites);
        _text_generator.generate(-104, 2, menu_index == 2 ? "> CREDITS" : "  CREDITS", _text_sprites);
        
        _accent_generator.generate(34, -61, "OPTIONS", _text_sprites);
        _text_generator.generate(34, -49, menu_index == 3 ? "> SFX" : "  SFX", _text_sprites);
        bn::string<16> sfx_text("  ");
        sfx_text += bn::to_string<4>(sfx_volume_percent);
        sfx_text += "%";
        _text_generator.generate(34, -39, sfx_text, _text_sprites);
        _text_generator.generate(34, -25, menu_index == 4 ? "> MUSIC" : "  MUSIC", _text_sprites);
        bn::string<16> music_text("  ");
        music_text += bn::to_string<4>(music_volume_percent);
        music_text += "%";
        _text_generator.generate(34, -15, music_text, _text_sprites);
        _text_generator.generate(34, -1, menu_index == 5 ? "> BLOCKS" : "  BLOCKS", _text_sprites);
        _text_generator.generate(34, 9, block_style_label(), _text_sprites);
        _text_generator.generate(34, 23, menu_index == 6 ? "> PALETTE" : "  PALETTE", _text_sprites);
        _text_generator.generate(34, 33, palette_style_label(), _text_sprites);

        const auto& entries = scores.entries();
        _accent_generator.generate(-104, 24, "BEST", _text_sprites);
        bn::string<32> best_text;
        best_text += entries[0].initials[0];
        best_text += entries[0].initials[1];
        best_text += entries[0].initials[2];
        best_text += " ";
        best_text += bn::to_string<10>(entries[0].score);
        _text_generator.generate(-104, 34, best_text, _text_sprites);

        _text_generator.generate(-104, 57, "UP/DOWN SELECT  L/R ADJUST", _text_sprites);
        _text_generator.generate(-104, 67, "A INC/CONFIRM  B DEC", _text_sprites);
        commit_frame();
    }

    void ui_renderer::render_high_scores(const high_scores& scores)
    {
        set_scene_background(scene_bg_type::info);
        clear_ui_map();
        _text_sprites.clear();

        _accent_generator.generate(-88, -58, "HIGH SCORES", _text_sprites);
        const auto& entries = scores.entries();

        for(int index = 0; index < high_scores::entries_count; ++index)
        {
            const auto& entry = entries[index];
            bn::string<32> line;
            line += bn::to_string<2>(index + 1);
            line += ". ";
            line += entry.initials[0];
            line += entry.initials[1];
            line += entry.initials[2];
            line += "  ";
            line += bn::to_string<10>(entry.score);
            _text_generator.generate(-88, -40 + (index * 12), line, _text_sprites);
        }

        _text_generator.generate(-88, 64, "A PLAY   B MENU", _text_sprites);
        commit_frame();
    }

    void ui_renderer::render_credits()
    {
        set_scene_background(scene_bg_type::info);
        clear_ui_map();
        _text_sprites.clear();

        _accent_generator.generate(-88, -58, "CREDITS", _text_sprites);
        _text_generator.generate(-88, -38, "BLOCKUDOKU GBA", _text_sprites);
        _text_generator.generate(-88, -28, "DESIGN + CODE", _text_sprites);
        _accent_generator.generate(-88, -18, "WILLIAM BEHNKE", _text_sprites);

        _text_generator.generate(-88, 2, "ENGINE: BUTANO", _text_sprites);
        _text_generator.generate(-88, 12, "PLATFORM: GAME BOY ADV", _text_sprites);
        _text_generator.generate(-88, 22, "MUSIC: FRUIT.MOD", _text_sprites);
        _text_generator.generate(-88, 32, "BY JESTER/SANITY", _text_sprites);
        _text_generator.generate(-88, 42, "ATTR-NC-SA", _text_sprites);

        _text_generator.generate(-88, 64, "A/B BACK", _text_sprites);
        commit_frame();
    }

    void ui_renderer::render_initials_entry(int score, const char initials[3], int selected_index)
    {
        set_scene_background(scene_bg_type::info);
        clear_ui_map();
        _text_sprites.clear();

        _accent_generator.generate(-88, -58, "NEW HIGH SCORE", _text_sprites);
        bn::string<24> score_text("SCORE ");
        score_text += bn::to_string<12>(score);
        _text_generator.generate(-88, -42, score_text, _text_sprites);

        _accent_generator.generate(-88, -16, "ENTER INITIALS", _text_sprites);
        bn::string<16> initials_text;
        initials_text += initials[0];
        initials_text += ' ';
        initials_text += initials[1];
        initials_text += ' ';
        initials_text += initials[2];
        _text_generator.generate(-88, 0, initials_text, _text_sprites);

        const int cursor_x = -88 + (selected_index * 16);
        _accent_generator.generate(cursor_x, 10, "^", _text_sprites);

        _text_generator.generate(-88, 54, "UP/DOWN LETTER", _text_sprites);
        _text_generator.generate(-88, 64, "L/R POS  A SAVE", _text_sprites);
        commit_frame();
    }

    void ui_renderer::build_static_bg()
    {
        _title_bg = bn::regular_bg_items::title_bg.create_bg(0, 0);
        _title_bg->set_priority(3);
        _title_bg->set_visible(false);

        _info_bg = bn::regular_bg_items::info_bg.create_bg(0, 0);
        _info_bg->set_priority(3);
        _info_bg->set_visible(false);

        _gameplay_bg = bn::regular_bg_items::gameplay_bg.create_bg(0, 0);
        _gameplay_bg->set_priority(3);
        _gameplay_bg->set_visible(false);

        for(int y = 0; y < ui_bg_map::rows; ++y)
        {
            for(int x = 0; x < ui_bg_map::columns; ++x)
            {
                bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(x, y)]);
                info.set_tile_index(tile_empty);
                info.set_palette_id(0);
                _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(x, y)] = info.cell();
            }
        }

        for(int y = board_bg_top - 1; y <= board_bg_top + 9; ++y)
        {
            for(int x = board_bg_left - 1; x <= board_bg_left + 9; ++x)
            {
                bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(x, y)]);
                info.set_tile_index(tile_empty);
                info.set_palette_id(0);
                _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(x, y)] = info.cell();
            }
        }

        for(int y = tray_bg_top - 1; y <= tray_bg_top + 11; ++y)
        {
            for(int x = tray_bg_left - 1; x <= tray_bg_left + 10; ++x)
            {
                bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(x, y)]);
                info.set_tile_index(tile_empty);
                info.set_palette_id(0);
                _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(x, y)] = info.cell();
            }
        }

        rebuild_ui_bg();
    }

    void ui_renderer::rebuild_ui_bg()
    {
        if(_ui_bg)
        {
            _ui_bg.reset();
            _ui_bg_map_ptr.reset();
        }

        const bn::regular_bg_tiles_item* tiles_item = &bn::regular_bg_tiles_items::block_type_classic;
        switch(_block_style)
        {
            case 1:
                tiles_item = &bn::regular_bg_tiles_items::block_type_strata;
                break;
            case 2:
                tiles_item = &bn::regular_bg_tiles_items::block_type_facet;
                break;
            default:
                tiles_item = &bn::regular_bg_tiles_items::block_type_classic;
                break;
        }

        const bn::bg_palette_item* palette_item = &bn::bg_palette_items::palette_classic;
        switch(_palette_style)
        {
            case 1:
                palette_item = &bn::bg_palette_items::palette_sunrise;
                break;
            case 2:
                palette_item = &bn::bg_palette_items::palette_mint;
                break;
            default:
                palette_item = &bn::bg_palette_items::palette_classic;
                break;
        }

        bn::regular_bg_item bg_item(*tiles_item, *palette_item, _ui_bg_map->map_item);

        _ui_bg = bg_item.create_bg(0, 0);
        _ui_bg->set_priority(2);
        _ui_bg->set_visible(true);
        _ui_bg_map_ptr = _ui_bg->map();
        _loaded_block_style = _block_style;
        _loaded_palette_style = _palette_style;
    }

    void ui_renderer::set_scene_background(scene_bg_type type)
    {
        if(_scene_bg_type == type)
        {
            return;
        }

        if(_ui_bg)
        {
            _ui_bg->set_visible(true);
        }

        if(_title_bg)
        {
            _title_bg->set_visible(type == scene_bg_type::title);
        }

        if(_info_bg)
        {
            _info_bg->set_visible(type == scene_bg_type::info);
        }

        if(_gameplay_bg)
        {
            _gameplay_bg->set_visible(type == scene_bg_type::gameplay);
        }

        _scene_bg_type = type;
    }

    void ui_renderer::draw_header(const game_state& state)
    {
        bn::string<20> score_text("SCORE ");
        score_text += bn::to_string<12>(state.score());
        _text_generator.generate(-116, -74, score_text, _text_sprites);

        bn::string<20> moves_text("MOVES ");
        moves_text += bn::to_string<12>(state.moves_available());
        _text_generator.generate(28, -74, moves_text, _text_sprites);
    }

    void ui_renderer::draw_board(const game_state& state)
    {
        bool green_highlight[game_state::board_size][game_state::board_size] = {};

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
                const int map_x = board_bg_left + x;
                const int map_y = board_bg_top + y;
                bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)]);

                int board_tile = tile_empty;

                if(state.board_cell(x, y))
                {
                    board_tile = green_highlight[y][x] ? tile_green : tile_tan;
                }

                info.set_tile_index(board_tile);
                info.set_palette_id(0);

                _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)] = info.cell();
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

                        const int map_x = board_bg_left + board_x;
                        const int map_y = board_bg_top + board_y;
                        bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)]);

                        int preview_tile = tile_blue;

                        if(valid && green_highlight[board_y][board_x])
                        {
                            preview_tile = tile_green;
                        }
                        else if(! valid && state.board_cell(board_x, board_y))
                        {
                            preview_tile = tile_red;
                        }

                        info.set_tile_index(preview_tile);
                        info.set_palette_id(0);
                        _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)] = info.cell();
                    }
                }
            }
        }
    }

    void ui_renderer::draw_tray(const game_state& state)
    {
        _accent_generator.generate(36, -56, "NEXT", _text_sprites);

        for(int slot = 0; slot < game_state::slot_count; ++slot)
        {
            const piece_def& piece = piece_library::at(state.slot_piece_index(slot));
            const bool selected = slot == state.selected_slot();

            const int slot_bg_y = tray_bg_top + (slot * 4);
            const int preview_left = tray_bg_left + 1;

            for(int y = 0; y < 4; ++y)
            {
                for(int x = 0; x < 5; ++x)
                {
                    const int map_x = preview_left + x;
                    const int map_y = slot_bg_y + y;
                    bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)]);

                    const bool active = state.slot_active(slot);
                    const bool filled = active && x < piece.width && y < piece.height && piece_library::cell(piece, x, y);
                    int tile_index = tile_empty;

                    if(filled)
                    {
                        if(! state.slot_can_place(slot))
                        {
                            tile_index = tile_red;
                        }
                        else if(selected)
                        {
                            tile_index = tile_blue;
                        }
                        else
                        {
                            tile_index = tile_tan;
                        }
                    }

                    info.set_tile_index(tile_index);
                    info.set_palette_id(0);
                    _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)] = info.cell();
                }
            }
        }
    }

    void ui_renderer::draw_controls(const game_state& state)
    {
        _text_generator.generate(-116, 62, "A PLACE  L/R NEXT", _text_sprites);

        if(state.game_over())
        {
            _text_generator.generate(-116, 72, "START RESET", _text_sprites);
        }
        else
        {
            _text_generator.generate(-116, 72, "SELECT RESET", _text_sprites);
        }
    }

    void ui_renderer::clear_ui_map()
    {
        for(int y = 0; y < ui_bg_map::rows; ++y)
        {
            for(int x = 0; x < ui_bg_map::columns; ++x)
            {
                bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(x, y)]);
                info.set_tile_index(tile_empty);
                info.set_palette_id(0);
                _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(x, y)] = info.cell();
            }
        }
    }

    void ui_renderer::commit_frame()
    {
        if(_ui_bg)
        {
            _ui_bg_map_ptr->reload_cells_ref();
        }
    }

    void ui_renderer::draw_title_blocks()
    {
        constexpr int base_x = 14;
        constexpr int base_y = 7;
        constexpr int shape[6][3] = {
            // x, y, tile
            { 0, 0, tile_tan },
            { 1, 0, tile_blue },
            { 2, 0, tile_blue },
            { 2, 1, tile_green },
            { 2, 2, tile_red },
            { 3, 2, tile_blue }
        };

        for(const auto& part : shape)
        {
            const int map_x = base_x + part[0];
            const int map_y = base_y + part[1];
            bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)]);
            info.set_tile_index(part[2]);
            info.set_palette_id(0);
            _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)] = info.cell();
        }
    }

    const char* ui_renderer::block_style_label() const
    {
        switch(_block_style)
        {
            case 0:
                return "  CLASSIC";
            case 1:
                return "  STRATA";
            case 2:
                return "  FACET";
            default:
                return "  CLASSIC";
        }
    }

    const char* ui_renderer::palette_style_label() const
    {
        switch(_palette_style)
        {
            case 0:
                return "  CLASSIC";
            case 1:
                return "  SUNRISE";
            case 2:
                return "  MINT";
            default:
                return "  CLASSIC";
        }
    }
}
