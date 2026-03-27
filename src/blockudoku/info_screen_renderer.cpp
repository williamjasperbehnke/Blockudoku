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

    void info_screen_renderer::render_credits(ui_renderer& renderer)
    {
        renderer.set_scene_background(ui_renderer::scene_bg_type::info);
        renderer.clear_ui_map();
        renderer._text_sprites.clear();

        renderer._accent_text_generator.generate(-88, -58, "CREDITS", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, -42, "PROJECT", renderer._text_sprites);
        renderer._text_generator.generate(-88, -32, "BLOCKUDOKU GBA", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, -18, "CREATED BY", renderer._text_sprites);
        renderer._text_generator.generate(-88, -8, "WILLIAM BEHNKE", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, 6, "TECH", renderer._text_sprites);
        renderer._text_generator.generate(-88, 16, "BUTANO ENGINE", renderer._text_sprites);
        renderer._text_generator.generate(-88, 26, "GAME BOY ADVANCE", renderer._text_sprites);

        renderer._accent_text_generator.generate(-88, 40, "MUSIC", renderer._text_sprites);
        renderer._text_generator.generate(-88, 50, "FRUIT.MOD  JESTER/SANITY", renderer._text_sprites);

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
