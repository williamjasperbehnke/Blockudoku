#ifndef BLOCKUDOKU_GAMEPLAY_SCREEN_RENDERER_H
#define BLOCKUDOKU_GAMEPLAY_SCREEN_RENDERER_H

namespace blockudoku
{

class game_state;
class ui_renderer;

class gameplay_screen_renderer
{
public:
    static void render(ui_renderer& renderer, const game_state& state);

private:
    static void draw_header(ui_renderer& renderer, const game_state& state);
    static void draw_board(ui_renderer& renderer, const game_state& state);
    static void draw_tray(ui_renderer& renderer, const game_state& state);
};

}

#endif
