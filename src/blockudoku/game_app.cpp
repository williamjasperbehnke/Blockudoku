#include "blockudoku/game_app.h"

#include "bn_keypad.h"

namespace blockudoku
{
    namespace
    {
        constexpr int max_volume_step = 10;
        constexpr int volume_step_count = max_volume_step + 1;
        constexpr bn::fixed music_volume_scale = bn::fixed(3) / 10;

        [[nodiscard]] int wrap_index(int value, int count)
        {
            return (value % count + count) % count;
        }

        [[nodiscard]] int percent_from_step(int volume_step)
        {
            return volume_step * 10;
        }

        [[nodiscard]] bn::fixed normalized_volume_from_step(int volume_step)
        {
            return bn::fixed(volume_step) / max_volume_step;
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
            _audio.set_sfx_volume(normalized_volume_from_step(_sfx_volume_step));
            _audio.set_music_volume(mapped_music_volume(_music_volume_step));
            _audio_initialized = true;
        }
    }

    bool game_app::selected_entry_is_option() const
    {
        return _menu_index >= int(menu_entry::sfx_volume);
    }

    void game_app::adjust_selected_option(int delta)
    {
        if(delta == 0)
        {
            return;
        }

        switch(static_cast<menu_entry>(_menu_index))
        {
            case menu_entry::sfx_volume:
                _sfx_volume_step = wrap_index(_sfx_volume_step + delta, volume_step_count);
                _audio.set_sfx_volume(normalized_volume_from_step(_sfx_volume_step));
                break;

            case menu_entry::music_volume:
                _music_volume_step = wrap_index(_music_volume_step + delta, volume_step_count);
                _audio.set_music_volume(mapped_music_volume(_music_volume_step));
                break;

            case menu_entry::blocks:
                _block_style = wrap_index(_block_style + delta, ui_renderer::block_style_count);
                break;

            case menu_entry::palette:
                _palette_style = wrap_index(_palette_style + delta, ui_renderer::palette_style_count);
                break;

            case menu_entry::assist:
                _assist_enabled = ! _assist_enabled;
                break;

            case menu_entry::start_game:
            case menu_entry::high_scores:
            case menu_entry::credits:
            case menu_entry::count:
                break;

            default:
                break;
        }
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
        constexpr int menu_options = int(menu_entry::count);
        bool option_adjusted = false;

        if(bn::keypad::up_pressed())
        {
            _menu_index = wrap_index(_menu_index - 1, menu_options);
            _audio.on_event({ game_event_type::slot_changed, 0 });
        }
        else if(bn::keypad::down_pressed())
        {
            _menu_index = wrap_index(_menu_index + 1, menu_options);
            _audio.on_event({ game_event_type::slot_changed, 0 });
        }

        int option_delta = 0;
        if(bn::keypad::left_pressed())
        {
            option_delta = -1;
        }
        else if(bn::keypad::right_pressed())
        {
            option_delta = 1;
        }
        else if(bn::keypad::b_pressed() && selected_entry_is_option())
        {
            option_delta = -1;
        }

        if(option_delta != 0 && selected_entry_is_option())
        {
            adjust_selected_option(option_delta);
            option_adjusted = true;
            _audio.on_event({ game_event_type::placed, 0 });
        }

        if(! option_adjusted && confirm_pressed())
        {
            switch(static_cast<menu_entry>(_menu_index))
            {
                case menu_entry::start_game:
                    start_game();
                    break;

                case menu_entry::high_scores:
                    _scene = scene::high_scores;
                    break;

                case menu_entry::credits:
                    _scene = scene::credits;
                    break;

                case menu_entry::sfx_volume:
                case menu_entry::music_volume:
                case menu_entry::blocks:
                case menu_entry::palette:
                case menu_entry::assist:
                    adjust_selected_option(1);
                    break;

                case menu_entry::count:
                    break;

                default:
                    break;
            }
            _audio.on_event({ game_event_type::placed, 0 });
        }

        _renderer.render_main_menu(
            _high_scores,
            _menu_index,
            percent_from_step(_sfx_volume_step),
            percent_from_step(_music_volume_step),
            _block_style,
            _palette_style,
            _assist_enabled);
    }

    void game_app::update_playing()
    {
        _renderer.set_block_style(_block_style);
        _renderer.set_palette_style(_palette_style);
        game_event event = { game_event_type::none, 0 };

        if(_assist_enabled)
        {
            if(bn::keypad::select_pressed())
            {
                _state.reset();
                _hint_service.reset();
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

            if(event.type == game_event_type::hint_requested)
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

        if(_high_scores.qualifies(_pending_score))
        {
            _initials[0] = 'A';
            _initials[1] = 'A';
            _initials[2] = 'A';
            _initials_index = 0;
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
            _initials_index = (_initials_index + 2) % 3;
        }
        else if(bn::keypad::right_pressed())
        {
            _initials_index = (_initials_index + 1) % 3;
        }
        else if(bn::keypad::up_pressed())
        {
            char& selected = _initials[_initials_index];
            selected = selected == 'Z' ? 'A' : char(selected + 1);
        }
        else if(bn::keypad::down_pressed())
        {
            char& selected = _initials[_initials_index];
            selected = selected == 'A' ? 'Z' : char(selected - 1);
        }

        if(confirm_pressed())
        {
            if(_initials_index < 2)
            {
                ++_initials_index;
            }
            else
            {
                _high_scores.insert(_initials, _pending_score);
                _scene = scene::high_scores;
            }
        }

        _renderer.render_initials_entry(_pending_score, _initials, _initials_index);
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
        _state.reset();
        _hint_service.reset();
        _scene = scene::playing;
    }
}
