#ifndef BLOCKUDOKU_MENU_SCREEN_RENDERER_H
#define BLOCKUDOKU_MENU_SCREEN_RENDERER_H

namespace blockudoku
{

class high_scores;
class ui_renderer;

class menu_screen_renderer
{
public:
    static void render(
            ui_renderer& renderer, const high_scores& scores, int menu_index, int sfx_volume_percent,
            int music_volume_percent, bool assist_enabled);

private:
    static void draw_title_blocks(ui_renderer& renderer);
};

}

#endif
