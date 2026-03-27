#include "blockudoku/info_screen_renderer.h"

#include "bn_string.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_achv_big_clear.h"
#include "bn_sprite_items_achv_combo_3.h"
#include "bn_sprite_items_achv_combo_4.h"
#include "bn_sprite_items_achv_combo_5.h"
#include "bn_sprite_items_achv_first_move.h"
#include "bn_sprite_items_achv_full_clear.h"
#include "bn_sprite_items_achv_line_clear.h"
#include "bn_sprite_items_achv_locked.h"
#include "bn_sprite_items_achv_score_1000.h"
#include "bn_sprite_items_achv_score_2000.h"
#include "bn_sprite_items_achv_score_3000.h"

#include "blockudoku/high_scores.h"
#include "blockudoku/ui_palette_provider.h"
#include "blockudoku/ui_render_constants.h"
#include "blockudoku/ui_renderer.h"

namespace blockudoku
{
    namespace
    {
        constexpr int visible_achievement_rows = 4;
    }

    void info_screen_renderer::render_high_scores(ui_renderer& renderer, const high_scores& scores)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._accent_text_generator.generate(-88, -58, "HIGH SCORES", renderer._text_sprites);
        const auto& entries = scores.entries();

        for(int index = 0; index < high_scores::entries_count; ++index)
        {
            const auto& entry = entries[index];
            bn::string<48> line;
            line += bn::to_string<2>(index + 1);
            line += ". ";
            line += entry.initials[0];
            line += entry.initials[1];
            line += entry.initials[2];
            line += " ";
            line += bn::to_string<10>(entry.score);
            renderer._text_generator.generate(-88, -40 + (index * 12), line, renderer._text_sprites);
        }

        renderer._accent_text_generator.generate(-88, 64, "A PLAY   B MENU", renderer._text_sprites);
        renderer.commit_frame();
    }

    void info_screen_renderer::render_achievements(ui_renderer& renderer, const high_scores& scores, int scroll_index)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        const int max_scroll = high_scores::achievements_count > visible_achievement_rows ?
                                       high_scores::achievements_count - visible_achievement_rows :
                                       0;
        if(scroll_index < 0)
        {
            scroll_index = 0;
        }
        else if(scroll_index > max_scroll)
        {
            scroll_index = max_scroll;
        }

        renderer._accent_text_generator.generate(-88, -58, "ACHIEVEMENTS", renderer._text_sprites);
        const bn::sprite_palette_ptr achievement_icon_palette =
                ui_palette_provider::text_palette_item(renderer._palette_style).create_palette();

        bn::string<24> progress_text;
        progress_text += bn::to_string<2>(scores.unlocked_achievements_count());
        progress_text += "/";
        progress_text += bn::to_string<2>(high_scores::achievements_count);
        progress_text += " UNLOCKED";
        renderer._text_generator.generate(-88, -46, progress_text, renderer._text_sprites);

        for(int row = 0; row < visible_achievement_rows; ++row)
        {
            const int achievement_index = scroll_index + row;
            if(achievement_index >= high_scores::achievements_count)
            {
                break;
            }

            const bool unlocked = scores.achievement_unlocked(achievement_index);
            const int icon_x = -80;
            const int icon_y = -28 + row * 24;

            if(unlocked)
            {
                switch(achievement_index)
                {
                    case int(high_scores::achievement_id::first_move):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_first_move.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::line_clear):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_line_clear.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::big_clear):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_big_clear.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::full_clear):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_full_clear.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::combo_3):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_combo_3.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::combo_4):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_combo_4.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::combo_5):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_combo_5.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::score_1000):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_score_1000.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::score_2000):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_score_2000.create_sprite(icon_x, icon_y));
                        break;
                    case int(high_scores::achievement_id::score_3000):
                        renderer._text_sprites.push_back(bn::sprite_items::achv_score_3000.create_sprite(icon_x, icon_y));
                        break;
                    default:
                        renderer._text_sprites.push_back(bn::sprite_items::achv_locked.create_sprite(icon_x, icon_y));
                        break;
                }
            }
            else
            {
                renderer._text_sprites.push_back(bn::sprite_items::achv_locked.create_sprite(icon_x, icon_y));
            }
            renderer._text_sprites.back().set_palette(achievement_icon_palette);

            renderer._text_generator.generate(
                    -62, -28 + row * 24, high_scores::achievement_name(achievement_index),
                    renderer._text_sprites);
        }

        const int bar_x = 29;
        const int bar_top = 7;
        const int bar_height = 18;
        for(int y = 0; y < bar_height; ++y)
        {
            renderer.set_ui_cell(bar_x, bar_top + y, ui_render_constants::tile_tan);
        }

        int thumb_height = (bar_height * visible_achievement_rows) / high_scores::achievements_count;
        if(thumb_height < 3)
        {
            thumb_height = 3;
        }

        const int travel = bar_height - thumb_height;
        int thumb_offset = 0;
        if(max_scroll > 0)
        {
            thumb_offset = (scroll_index * travel) / max_scroll;
        }

        for(int y = 0; y < thumb_height; ++y)
        {
            renderer.set_ui_cell(bar_x, bar_top + thumb_offset + y, ui_render_constants::tile_green);
        }

        renderer._accent_text_generator.generate(-88, 64, "UP/DOWN SCROLL  B BACK", renderer._text_sprites);
        renderer.commit_frame();
    }

    void info_screen_renderer::render_credits(ui_renderer& renderer)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._accent_text_generator.generate(-88, -58, "CREDITS", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, -42, "CREATED BY", renderer._text_sprites);
        renderer._text_generator.generate(-88, -32, "WILLIAM BEHNKE", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, -18, "TECH", renderer._text_sprites);
        renderer._text_generator.generate(-88, -8, "BUTANO ENGINE", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, 6, "MUSIC", renderer._text_sprites);
        renderer._text_generator.generate(-88, 16, "FRUIT.MOD, JESTER/SANITY", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, 30, "ACHIEVEMENT ICONS", renderer._text_sprites);
        renderer._text_generator.generate(-88, 40, "PIXEL ICONS, NIKOICHU", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, 64, "A/B BACK", renderer._text_sprites);
        renderer.commit_frame();
    }

    void info_screen_renderer::render_initials_entry(
            ui_renderer& renderer, int score, const char initials[3], int selected_index)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._accent_text_generator.generate(-88, -58, "NEW HIGH SCORE", renderer._text_sprites);
        bn::string<24> score_text("SCORE ");
        score_text += bn::to_string<12>(score);
        renderer._text_generator.generate(-88, -42, score_text, renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, -16, "ENTER INITIALS", renderer._text_sprites);
        bn::string<16> initials_text;
        initials_text += initials[0];
        initials_text += ' ';
        initials_text += initials[1];
        initials_text += ' ';
        initials_text += initials[2];
        renderer._text_generator.generate(-88, 0, initials_text, renderer._text_sprites);

        const int cursor_x = -88 + (selected_index * 16);
        renderer._text_generator.generate(cursor_x, 10, "^", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, 54, "UP/DOWN LETTER", renderer._text_sprites);
        renderer._accent_text_generator.generate(-88, 64, "L/R POS  A SAVE", renderer._text_sprites);
        renderer.commit_frame();
    }

    void info_screen_renderer::render_seed_entry(ui_renderer& renderer, const char seed_digits[8], int selected_index)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._accent_text_generator.generate(-88, -58, "SET RUN SEED", renderer._text_sprites);
        renderer._accent_text_generator.generate(-88, -40, "PRESS A TO START", renderer._text_sprites);

        bn::string<24> seed_text("SEED ");
        for(int index = 0; index < ui_render_constants::seed_digits_count; ++index)
        {
            seed_text += seed_digits[index];
        }
        renderer._text_generator.generate(-88, -12, seed_text, renderer._text_sprites);

        const int cursor_x = -88 + 5 * 8 + (selected_index * 8);
        renderer._accent_text_generator.generate(cursor_x, 0, "^", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, 64, "UP/DOWN DIGIT  L/R POS", renderer._text_sprites);
        renderer.commit_frame();
    }

    void info_screen_renderer::render_resume_prompt(
            ui_renderer& renderer, bool continue_selected, int saved_score, unsigned saved_seed)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._accent_text_generator.generate(-88, -58, "RESUME RUN?", renderer._text_sprites);

        bn::string<24> score_text("SCORE ");
        score_text += bn::to_string<12>(saved_score);
        renderer._text_generator.generate(-88, -40, score_text, renderer._text_sprites);

        bn::string<24> seed_text("SEED ");
        seed_text += bn::to_string<12>(saved_seed);
        renderer._text_generator.generate(-88, -28, seed_text, renderer._text_sprites);

        (continue_selected ? renderer._accent_text_generator : renderer._text_generator)
                .generate(-88, -4, continue_selected ? "> CONTINUE GAME" : "  CONTINUE GAME", renderer._text_sprites);
        (! continue_selected ? renderer._accent_text_generator : renderer._text_generator)
                .generate(-88, 8, ! continue_selected ? "> START NEW GAME" : "  START NEW GAME", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, 64, "UP/DOWN CHOOSE  A BEGIN", renderer._text_sprites);
        renderer.commit_frame();
    }
}
