#include "blockudoku/menu_screen_renderer.h"

#include "bn_string.h"

#include "blockudoku/high_scores.h"
#include "blockudoku/ui_render_constants.h"
#include "blockudoku/ui_renderer.h"

namespace blockudoku
{
    void menu_screen_renderer::draw_title_blocks(ui_renderer& renderer)
    {
        constexpr int base_x = 14;
        constexpr int base_y = 9;
        constexpr int shape[6][3] = {
            { 0, 0, ui_render_constants::tile_tan },
            { 1, 0, ui_render_constants::tile_blue },
            { 2, 0, ui_render_constants::tile_blue },
            { 2, 1, ui_render_constants::tile_green },
            { 2, 2, ui_render_constants::tile_red },
            { 3, 2, ui_render_constants::tile_blue }
        };

        for(const auto& part : shape)
        {
            renderer.set_ui_cell(base_x + part[0], base_y + part[1], part[2]);
        }
    }

    void menu_screen_renderer::render(
            ui_renderer& renderer, const high_scores& scores, int menu_index, int sfx_volume_percent,
            int music_volume_percent, bool assist_enabled)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::title);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();
        draw_title_blocks(renderer);

        renderer._text_generator.generate(-104, -52, "BLOCKUDOKU", renderer._text_sprites);
        renderer._text_generator.generate(-104, -36, "GBA EDITION", renderer._text_sprites);

        renderer._text_generator.generate(-104, -12, "MENU", renderer._text_sprites);
        renderer._text_generator.generate(
                -104, -2, menu_index == 0 ? "> START GAME" : "  START GAME", renderer._text_sprites);
        renderer._text_generator.generate(
                -104, 8, menu_index == 1 ? "> HIGH SCORES" : "  HIGH SCORES", renderer._text_sprites);
        renderer._text_generator.generate(
                -104, 18, menu_index == 2 ? "> CREDITS" : "  CREDITS", renderer._text_sprites);

        renderer._text_generator.generate(34, -55, "OPTIONS", renderer._text_sprites);
        renderer._text_generator.generate(34, -43, menu_index == 3 ? "> SFX" : "  SFX", renderer._text_sprites);
        bn::string<16> sfx_text("  ");
        sfx_text += bn::to_string<4>(sfx_volume_percent);
        sfx_text += "%";
        renderer._text_generator.generate(34, -33, sfx_text, renderer._text_sprites);

        renderer._text_generator.generate(
                34, -19, menu_index == 4 ? "> MUSIC" : "  MUSIC", renderer._text_sprites);
        bn::string<16> music_text("  ");
        music_text += bn::to_string<4>(music_volume_percent);
        music_text += "%";
        renderer._text_generator.generate(34, -9, music_text, renderer._text_sprites);

        renderer._text_generator.generate(34, 5, menu_index == 5 ? "> BLOCKS" : "  BLOCKS", renderer._text_sprites);
        renderer._text_generator.generate(34, 15, renderer.block_style_label(), renderer._text_sprites);

        renderer._text_generator.generate(
                34, 29, menu_index == 6 ? "> PALETTE" : "  PALETTE", renderer._text_sprites);
        renderer._text_generator.generate(34, 39, renderer.palette_style_label(), renderer._text_sprites);

        renderer._text_generator.generate(34, 53, menu_index == 7 ? "> ASSIST" : "  ASSIST", renderer._text_sprites);
        renderer._text_generator.generate(34, 63, assist_enabled ? "  ON" : "  OFF", renderer._text_sprites);

        const auto& entries = scores.entries();
        renderer._text_generator.generate(-104, 40, "BEST", renderer._text_sprites);
        bn::string<32> best_text;
        best_text += entries[0].initials[0];
        best_text += entries[0].initials[1];
        best_text += entries[0].initials[2];
        best_text += " ";
        best_text += bn::to_string<10>(entries[0].score);
        renderer._text_generator.generate(-104, 50, best_text, renderer._text_sprites);

        bn::string<32> seed_text("SEED ");
        seed_text += bn::to_string<12>(entries[0].seed);
        renderer._text_generator.generate(-104, 60, seed_text, renderer._text_sprites);

        renderer.commit_frame();
    }
}
