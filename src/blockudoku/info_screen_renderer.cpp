#include "blockudoku/info_screen_renderer.h"

#include "bn_string.h"

#include "blockudoku/high_scores.h"
#include "blockudoku/ui_render_constants.h"
#include "blockudoku/ui_renderer.h"

namespace blockudoku
{
    void info_screen_renderer::render_high_scores(ui_renderer& renderer, const high_scores& scores)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._text_generator.generate(-88, -58, "HIGH SCORES", renderer._text_sprites);
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
            line += " S";
            line += bn::to_string<12>(entry.seed);
            renderer._text_generator.generate(-88, -40 + (index * 12), line, renderer._text_sprites);
        }

        renderer._text_generator.generate(-88, 64, "A PLAY   B MENU", renderer._text_sprites);
        renderer.commit_frame();
    }

    void info_screen_renderer::render_credits(ui_renderer& renderer)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._text_generator.generate(-88, -58, "CREDITS", renderer._text_sprites);
        renderer._text_generator.generate(-88, -38, "BLOCKUDOKU GBA", renderer._text_sprites);
        renderer._text_generator.generate(-88, -28, "DESIGN + CODE", renderer._text_sprites);
        renderer._text_generator.generate(-88, -18, "WILLIAM BEHNKE", renderer._text_sprites);

        renderer._text_generator.generate(-88, 2, "ENGINE: BUTANO", renderer._text_sprites);
        renderer._text_generator.generate(-88, 12, "PLATFORM: GAME BOY ADV", renderer._text_sprites);
        renderer._text_generator.generate(-88, 22, "MUSIC: FRUIT.MOD", renderer._text_sprites);
        renderer._text_generator.generate(-88, 32, "BY JESTER/SANITY", renderer._text_sprites);
        renderer._text_generator.generate(-88, 42, "ATTR-NC-SA", renderer._text_sprites);

        renderer._text_generator.generate(-88, 64, "A/B BACK", renderer._text_sprites);
        renderer.commit_frame();
    }

    void info_screen_renderer::render_initials_entry(
            ui_renderer& renderer, int score, const char initials[3], int selected_index)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._text_generator.generate(-88, -58, "NEW HIGH SCORE", renderer._text_sprites);
        bn::string<24> score_text("SCORE ");
        score_text += bn::to_string<12>(score);
        renderer._text_generator.generate(-88, -42, score_text, renderer._text_sprites);

        renderer._text_generator.generate(-88, -16, "ENTER INITIALS", renderer._text_sprites);
        bn::string<16> initials_text;
        initials_text += initials[0];
        initials_text += ' ';
        initials_text += initials[1];
        initials_text += ' ';
        initials_text += initials[2];
        renderer._text_generator.generate(-88, 0, initials_text, renderer._text_sprites);

        const int cursor_x = -88 + (selected_index * 16);
        renderer._text_generator.generate(cursor_x, 10, "^", renderer._text_sprites);

        renderer._text_generator.generate(-88, 54, "UP/DOWN LETTER", renderer._text_sprites);
        renderer._text_generator.generate(-88, 64, "L/R POS  A SAVE", renderer._text_sprites);
        renderer.commit_frame();
    }

    void info_screen_renderer::render_seed_entry(ui_renderer& renderer, const char seed_digits[8], int selected_index)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._text_generator.generate(-88, -58, "SET RUN SEED", renderer._text_sprites);
        renderer._text_generator.generate(-88, -40, "PRESS A TO START", renderer._text_sprites);

        bn::string<24> seed_text("SEED ");
        for(int index = 0; index < ui_render_constants::seed_digits_count; ++index)
        {
            seed_text += seed_digits[index];
        }
        renderer._text_generator.generate(-88, -12, seed_text, renderer._text_sprites);

        const int cursor_x = -88 + 5 * 8 + (selected_index * 8);
        renderer._selected_tray_generator.generate(cursor_x, 0, "^", renderer._text_sprites);

        renderer._text_generator.generate(-88, 64, "UP/DOWN DIGIT  L/R POS", renderer._text_sprites);
        renderer.commit_frame();
    }
}
