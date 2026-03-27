#ifndef BLOCKUDOKU_INFO_SCREEN_RENDERER_H
#define BLOCKUDOKU_INFO_SCREEN_RENDERER_H

namespace blockudoku
{

class high_scores;
class ui_renderer;

class info_screen_renderer
{
public:
    static void render_high_scores(ui_renderer& renderer, const high_scores& scores);
    static void render_achievements(ui_renderer& renderer, const high_scores& scores, int scroll_index);
    static void render_credits(ui_renderer& renderer);
    static void render_initials_entry(ui_renderer& renderer, int score, const char initials[3], int selected_index);
    static void render_seed_entry(ui_renderer& renderer, const char seed_digits[8], int selected_index);
    static void render_resume_prompt(
            ui_renderer& renderer, bool continue_selected, int saved_score, unsigned saved_seed);
};

}

#endif
