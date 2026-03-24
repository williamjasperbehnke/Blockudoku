#ifndef BLOCKUDOKU_GAME_APP_H
#define BLOCKUDOKU_GAME_APP_H

#include "blockudoku/game_audio.h"
#include "blockudoku/game_state.h"
#include "blockudoku/high_scores.h"
#include "blockudoku/hint_service.h"
#include "blockudoku/initials_entry_controller.h"
#include "blockudoku/input_controller.h"
#include "blockudoku/menu_controller.h"
#include "blockudoku/run_seed_controller.h"
#include "blockudoku/ui_renderer.h"

namespace blockudoku
{

class game_app
{
public:
    void update();

private:
    enum class scene
    {
        menu,
        playing,
        enter_seed,
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
    menu_controller _menu;
    bool _audio_initialized = false;
    hint_service _hint_service;
    run_seed_controller _run_seed;
    initials_entry_controller _initials_entry;
    int _pending_score = 0;
    unsigned _pending_seed = 0;

    void initialize_audio_once();
    void open_seed_entry();

    void update_menu();
    void update_playing();
    void update_enter_seed();
    void update_enter_initials();
    void update_high_scores();
    void update_credits();
    void start_game_with_seed(unsigned seed);
    void start_game();
};

}

#endif
