#include "blockudoku/ui_renderer.h"

#include "bn_memory.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_regular_bg_map_ptr.h"

#include "bn_regular_bg_items_gameplay_bg.h"
#include "bn_regular_bg_items_info_bg.h"
#include "bn_regular_bg_items_title_bg.h"
#include "bn_regular_bg_tiles_items_block_type_brick.h"
#include "bn_regular_bg_tiles_items_block_type_classic.h"
#include "bn_regular_bg_tiles_items_block_type_face.h"
#include "bn_regular_bg_tiles_items_block_type_minecraft.h"
#include "bn_regular_bg_tiles_items_block_type_flat.h"

#include "common_fixed_8x8_sprite_font.h"

#include "blockudoku/game_state.h"
#include "blockudoku/gameplay_screen_renderer.h"
#include "blockudoku/info_screen_renderer.h"
#include "blockudoku/menu_screen_renderer.h"
#include "blockudoku/ui_palette_provider.h"
#include "blockudoku/ui_render_constants.h"

namespace blockudoku
{

    ui_renderer::ui_bg_map::ui_bg_map() :
        map_item(cells[0], bn::size(columns, rows))
    {
        bn::memory::clear(cells);
    }

    ui_renderer::ui_renderer() :
        _ui_bg_map(new ui_bg_map()),
        _text_generator(common::fixed_8x8_sprite_font),
        _accent_text_generator(
                common::fixed_8x8_sprite_font,
                ui_palette_provider::accent_palette_item(palette_style::classic))
    {
        _text_generator.set_left_alignment();
        _accent_text_generator.set_left_alignment();
        _text_generator.set_bg_priority(0);
        _accent_text_generator.set_bg_priority(0);
        _text_generator.set_z_order(-100);
        _accent_text_generator.set_z_order(-100);
        apply_text_palette();
        build_static_bg();
    }

    void ui_renderer::set_block_style(block_style style)
    {
        _block_style = style;
        if(_block_style != _loaded_block_style)
        {
            rebuild_ui_bg();
        }
    }

    void ui_renderer::set_palette_style(palette_style style)
    {
        _palette_style = style;
        if(_palette_style != _loaded_palette_style)
        {
            rebuild_ui_bg();
            apply_background_palette();
            apply_text_palette();
        }
    }

    void ui_renderer::trigger_clear_feedback(int cleared_cells, bool full_board_clear)
    {
        _feedback.on_clear(cleared_cells, full_board_clear);
    }

    void ui_renderer::render(const game_state& state, bool dev_mode)
    {
        gameplay_screen_renderer::render(*this, state, dev_mode);
    }

    void ui_renderer::render_main_menu(
            const high_scores& scores, int menu_index, int sfx_volume_percent, int music_volume_percent,
            block_style style, palette_style palette, bool assist_enabled)
    {
        set_block_style(style);
        set_palette_style(palette);
        menu_screen_renderer::render(*this, scores, menu_index, sfx_volume_percent, music_volume_percent, assist_enabled);
    }

    void ui_renderer::render_high_scores(const high_scores& scores)
    {
        info_screen_renderer::render_high_scores(*this, scores);
    }

    void ui_renderer::render_achievements(const high_scores& scores, int scroll_index)
    {
        info_screen_renderer::render_achievements(*this, scores, scroll_index);
    }

    void ui_renderer::render_credits()
    {
        info_screen_renderer::render_credits(*this);
    }

    void ui_renderer::render_seed_entry(const char seed_digits[8], int selected_index)
    {
        info_screen_renderer::render_seed_entry(*this, seed_digits, selected_index);
    }

    void ui_renderer::render_initials_entry(int score, const char initials[3], int selected_index)
    {
        info_screen_renderer::render_initials_entry(*this, score, initials, selected_index);
    }

    void ui_renderer::render_resume_prompt(bool continue_selected, int saved_score, unsigned saved_seed)
    {
        info_screen_renderer::render_resume_prompt(*this, continue_selected, saved_score, saved_seed);
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

        clear_ui_region(0, 0, ui_bg_map::columns - 1, ui_bg_map::rows - 1);
        clear_ui_region(
                ui_render_constants::board_bg_left - 1,
                ui_render_constants::board_bg_top - 1,
                ui_render_constants::board_bg_left + (game_state::board_size * ui_render_constants::board_cell_tiles),
                ui_render_constants::board_bg_top + (game_state::board_size * ui_render_constants::board_cell_tiles));
        clear_ui_region(
                ui_render_constants::tray_bg_left - 1,
                ui_render_constants::tray_bg_top - 2,
                ui_render_constants::tray_bg_left + 9,
                ui_render_constants::tray_bg_top + 11);

        rebuild_ui_bg();
        apply_background_palette();
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
            case block_style::brick:
                tiles_item = &bn::regular_bg_tiles_items::block_type_brick;
                break;
            case block_style::flat:
                tiles_item = &bn::regular_bg_tiles_items::block_type_flat;
                break;
            case block_style::minecraft:
                tiles_item = &bn::regular_bg_tiles_items::block_type_minecraft;
                break;
            case block_style::face:
                tiles_item = &bn::regular_bg_tiles_items::block_type_face;
                break;
            case block_style::classic:
            case block_style::count:
            default:
                tiles_item = &bn::regular_bg_tiles_items::block_type_classic;
                break;
        }

        const bn::bg_palette_item* palette_item = &ui_palette_provider::background_palette_item(_palette_style);

        bn::regular_bg_item bg_item(*tiles_item, *palette_item, _ui_bg_map->map_item);

        _ui_bg = bg_item.create_bg(0, 0);
        _ui_bg->set_priority(2);
        _ui_bg->set_visible(true);
        _ui_bg_map_ptr = _ui_bg->map();
        _loaded_block_style = _block_style;
        _loaded_palette_style = _palette_style;
    }

    void ui_renderer::apply_background_palette()
    {
        bn::bg_palette_ptr palette = ui_palette_provider::create_background_palette(_palette_style);

        if(_title_bg)
        {
            _title_bg->set_palette(palette);
        }

        if(_info_bg)
        {
            _info_bg->set_palette(palette);
        }

        if(_gameplay_bg)
        {
            _gameplay_bg->set_palette(palette);
        }
    }

    void ui_renderer::apply_text_palette()
    {
        _text_generator.set_palette_item(ui_palette_provider::text_palette_item(_palette_style));
        _accent_text_generator.set_palette_item(ui_palette_provider::accent_palette_item(_palette_style));
    }

    void ui_renderer::set_scene_background(scene_bg_type type)
    {
        _feedback.on_scene_change(type == scene_bg_type::gameplay, _gameplay_bg, _ui_bg);

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

    void ui_renderer::clear_ui_map()
    {
        clear_ui_region(0, 0, ui_bg_map::columns - 1, ui_bg_map::rows - 1);
    }

    void ui_renderer::set_ui_cell(int map_x, int map_y, int tile_index)
    {
        bn::regular_bg_map_cell_info info(_ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)]);
        info.set_tile_index(tile_index);
        info.set_palette_id(0);
        _ui_bg_map->cells[_ui_bg_map->map_item.cell_index(map_x, map_y)] = info.cell();
    }

    void ui_renderer::clear_ui_region(int left, int top, int right, int bottom)
    {
        for(int y = top; y <= bottom; ++y)
        {
            for(int x = left; x <= right; ++x)
            {
                set_ui_cell(x, y, ui_render_constants::tile_empty);
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

    const char* ui_renderer::block_style_label() const
    {
        switch(_block_style)
        {
            case block_style::brick:
                return "  BRICK";
            case block_style::flat:
                return "  FLAT";
            case block_style::minecraft:
                return "  MINECRFT";
            case block_style::face:
                return "  FACE";
            default:
                return "  CLASSIC";
        }
    }

    const char* ui_renderer::palette_style_label() const
    {
        switch(_palette_style)
        {
            case palette_style::sunrise:
                return "  SUNRISE";
            case palette_style::mint:
                return "  MINT";
            case palette_style::ocean:
                return "  OCEAN";
            case palette_style::forest:
                return "  FOREST";
            case palette_style::colorblind:
                return "  CBLIND";
            case palette_style::grayscale:
                return "  GRAYSCL";
            case palette_style::gameboy:
                return "  GAMEBOY";
            default:
                return "  CLASSIC";
        }
    }
}
