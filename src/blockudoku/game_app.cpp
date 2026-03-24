#include "blockudoku/game_app.h"

#include "bn_core.h"
#include "bn_keypad.h"

namespace blockudoku
{
    namespace
    {
        constexpr bn::fixed music_volume_scale = bn::fixed(3) / 10;

        [[nodiscard]] int percent_from_step(int volume_step)
        {
            return volume_step * 10;
        }

        [[nodiscard]] bn::fixed normalized_volume_from_step(int volume_step)
        {
            return bn::fixed(volume_step) / menu_controller::max_volume_step;
        }

        [[nodiscard]] bn::fixed mapped_music_volume(int music_volume_step)
        {
            return normalized_volume_from_step(music_volume_step) * music_volume_scale;
        }

        [[nodiscard]] bool confirm_pressed()
        {
            return bn::keypad::a_pressed() || bn::keypad::start_pressed();
        }

        [[nodiscard]] bool cancel_pressed()
        {
            return bn::keypad::b_pressed() || bn::keypad::select_pressed();
        }
    }

    void game_app::initialize_audio_once()
    {
        if(! _audio_initialized)
        {
            _audio.set_sfx_volume(normalized_volume_from_step(_menu.sfx_volume_step()));
            _audio.set_music_volume(mapped_music_volume(_menu.music_volume_step()));
            _audio_initialized = true;
        }
    }

    void game_app::open_seed_entry()
    {
        _run_seed.begin_manual_entry();
        _scene = scene::enter_seed;
    }

    void game_app::update()
    {
        initialize_audio_once();

        _audio.update_music();

        switch(_scene)
        {
            case scene::menu:
                update_menu();
                break;

            case scene::playing:
                update_playing();
                break;

            case scene::enter_seed:
                update_enter_seed();
                break;

            case scene::enter_initials:
                update_enter_initials();
                break;

            case scene::high_scores:
                update_high_scores();
                break;

            case scene::credits:
                update_credits();
                break;

            default:
                _scene = scene::menu;
                break;
        }
    }

    void game_app::update_menu()
    {
        const menu_controller::update_result menu_update =
                _menu.update(ui_renderer::block_style_count, ui_renderer::palette_style_count);

        if(menu_update.selection_changed)
        {
            _audio.on_event({ game_event_type::slot_changed, 0 });
        }

        if(menu_update.sfx_volume_changed)
        {
            _audio.set_sfx_volume(normalized_volume_from_step(_menu.sfx_volume_step()));
        }

        if(menu_update.music_volume_changed)
        {
            _audio.set_music_volume(mapped_music_volume(_menu.music_volume_step()));
        }

        switch(menu_update.next_action)
        {
            case menu_controller::action::start_game:
                start_game();
                break;

            case menu_controller::action::open_seed_entry:
                open_seed_entry();
                break;

            case menu_controller::action::show_high_scores:
                _scene = scene::high_scores;
                break;

            case menu_controller::action::show_credits:
                _scene = scene::credits;
                break;

            case menu_controller::action::none:
                break;

            default:
                break;
        }

        if(menu_update.option_changed || menu_update.next_action != menu_controller::action::none)
        {
            _audio.on_event({ game_event_type::placed, 0 });
        }

        _renderer.render_main_menu(
            _high_scores,
            _menu.menu_index(),
            percent_from_step(_menu.sfx_volume_step()),
            percent_from_step(_menu.music_volume_step()),
            _menu.block_style(),
            _menu.palette_style(),
            _menu.assist_enabled());
    }

    void game_app::update_playing()
    {
        _renderer.set_block_style(_menu.block_style());
        _renderer.set_palette_style(_menu.palette_style());
        game_event event = { game_event_type::none, 0 };

        if(_menu.assist_enabled())
        {
            if(bn::keypad::select_pressed())
            {
                start_game();
                event = { game_event_type::reset, 0 };
            }
            else
            {
                event = _hint_service.run_assist_step(_state);
            }
        }
        else
        {
            event = _input.update(_state);

            if(event.type == game_event_type::reset)
            {
                start_game();
            }
            else if(event.type == game_event_type::hint_requested)
            {
                _hint_service.request_manual(_state);
            }
            else if(event.type != game_event_type::none)
            {
                _hint_service.cancel_manual();
            }

            _hint_service.update_manual(_state, event);
        }

        if(event.cleared_cells > 0)
        {
            _renderer.trigger_clear_feedback(event.cleared_cells);
        }

        _renderer.render(_state);
        _audio.on_event(event);

        if(event.type != game_event_type::game_over)
        {
            return;
        }

        _pending_score = _state.score();
        _pending_seed = _state.run_seed();

        if(_high_scores.qualifies(_pending_score))
        {
            _initials_entry.begin();
            _scene = scene::enter_initials;
        }
        else
        {
            _scene = scene::high_scores;
        }
    }

    void game_app::update_enter_initials()
    {
        if(bn::keypad::left_pressed())
        {
            _initials_entry.move_cursor(-1);
        }
        else if(bn::keypad::right_pressed())
        {
            _initials_entry.move_cursor(1);
        }
        else if(bn::keypad::up_pressed())
        {
            _initials_entry.adjust_selected_letter(1);
        }
        else if(bn::keypad::down_pressed())
        {
            _initials_entry.adjust_selected_letter(-1);
        }

        if(confirm_pressed() && _initials_entry.confirm())
        {
            _high_scores.insert(_initials_entry.initials(), _pending_score, _pending_seed);
            _scene = scene::high_scores;
        }

        _renderer.render_initials_entry(
                _pending_score, _initials_entry.initials(), _initials_entry.cursor_index());
    }

    void game_app::update_enter_seed()
    {
        if(bn::keypad::left_pressed())
        {
            _run_seed.move_manual_cursor(-1);
        }
        else if(bn::keypad::right_pressed())
        {
            _run_seed.move_manual_cursor(1);
        }
        else if(bn::keypad::up_pressed())
        {
            _run_seed.adjust_manual_digit(1);
        }
        else if(bn::keypad::down_pressed())
        {
            _run_seed.adjust_manual_digit(-1);
        }

        if(confirm_pressed())
        {
            start_game_with_seed(_run_seed.manual_seed());
            return;
        }

        if(cancel_pressed())
        {
            _scene = scene::menu;
            return;
        }

        _renderer.render_seed_entry(_run_seed.manual_digits(), _run_seed.manual_cursor_index());
    }

    void game_app::update_high_scores()
    {
        if(confirm_pressed())
        {
            start_game();
        }
        else if(cancel_pressed())
        {
            _scene = scene::menu;
        }

        _renderer.render_high_scores(_high_scores);
    }

    void game_app::update_credits()
    {
        if(cancel_pressed() || confirm_pressed())
        {
            _scene = scene::menu;
        }

        _renderer.render_credits();
    }

    void game_app::start_game()
    {
        const unsigned entropy = unsigned(bn::core::current_cpu_ticks());
        start_game_with_seed(_run_seed.next_auto_seed(entropy));
    }

    void game_app::start_game_with_seed(unsigned seed)
    {
        _state.set_run_seed(seed);
        _state.reset();
        _hint_service.reset();
        _scene = scene::playing;
    }
}
