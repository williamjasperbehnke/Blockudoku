#ifndef BLOCKUDOKU_GAME_APP_H
#define BLOCKUDOKU_GAME_APP_H

#include "blockudoku/game_audio.h"
#include "blockudoku/game_state.h"
#include "blockudoku/high_scores.h"
#include "blockudoku/hint_service.h"
#include "blockudoku/input_controller.h"
#include "blockudoku/ui_renderer.h"

namespace blockudoku
{

class game_app
{
public:
    void update();

private:
    enum class menu_entry
    {
        start_game = 0,
        high_scores = 1,
        credits = 2,
        sfx_volume = 3,
        music_volume = 4,
        blocks = 5,
        palette = 6,
        assist = 7,
        count = 8
    };

    enum class scene
    {
        menu,
        playing,
        enter_initials,
        high_scores,
        credits
    };

    scene _scene = scene::menu;
    game_state _state;
    input_controller _input;
    ui_renderer _renderer;
    game_audio _audio;
    high_scores _high_scores;
    int _menu_index = 0;
    int _sfx_volume_step = 10;
    int _music_volume_step = 10;
    bool _audio_initialized = false;
    int _block_style = 0;
    int _palette_style = 0;
    bool _assist_enabled = false;
    hint_service _hint_service;
    char _initials[3] = { 'A', 'A', 'A' };
    int _initials_index = 0;
    int _pending_score = 0;

    void initialize_audio_once();
    [[nodiscard]] bool selected_entry_is_option() const;
    void adjust_selected_option(int delta);

    void update_menu();
    void update_playing();
    void update_enter_initials();
    void update_high_scores();
    void update_credits();
    void start_game();
};

}

#endif
