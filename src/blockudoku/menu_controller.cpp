#include "blockudoku/menu_controller.h"

#include "bn_keypad.h"

namespace blockudoku
{
    namespace
    {
        [[nodiscard]] int wrap_index(int value, int count)
        {
            return (value % count + count) % count;
        }

        [[nodiscard]] bool confirm_pressed()
        {
            return bn::keypad::a_pressed() || bn::keypad::start_pressed();
        }
    }

    menu_controller::update_result menu_controller::update(int block_style_count, int palette_style_count)
    {
        update_result result;

        if(bn::keypad::up_pressed())
        {
            _menu_index = wrap_index(_menu_index - 1, int(entry::count));
            result.selection_changed = true;
        }
        else if(bn::keypad::down_pressed())
        {
            _menu_index = wrap_index(_menu_index + 1, int(entry::count));
            result.selection_changed = true;
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
            adjust_selected_option(option_delta, block_style_count, palette_style_count, result);
        }

        if(! result.option_changed && confirm_pressed())
        {
            switch(selected_entry())
            {
                case entry::start_game:
                    result.next_action = action::start_game;
                    break;

                case entry::high_scores:
                    result.next_action = action::show_high_scores;
                    break;

                case entry::credits:
                    result.next_action = action::show_credits;
                    break;

                case entry::sfx_volume:
                case entry::music_volume:
                case entry::blocks:
                case entry::palette:
                case entry::assist:
                    adjust_selected_option(1, block_style_count, palette_style_count, result);
                    break;

                case entry::count:
                    break;

                default:
                    break;
            }
        }

        if(bn::keypad::select_pressed() && selected_entry() == entry::start_game)
        {
            result.next_action = action::open_seed_entry;
        }

        return result;
    }

    int menu_controller::menu_index() const
    {
        return _menu_index;
    }

    int menu_controller::sfx_volume_step() const
    {
        return _sfx_volume_step;
    }

    int menu_controller::music_volume_step() const
    {
        return _music_volume_step;
    }

    int menu_controller::block_style() const
    {
        return _block_style;
    }

    int menu_controller::palette_style() const
    {
        return _palette_style;
    }

    bool menu_controller::assist_enabled() const
    {
        return _assist_enabled;
    }

    bool menu_controller::selected_entry_is_option() const
    {
        return _menu_index >= int(entry::sfx_volume);
    }

    menu_controller::entry menu_controller::selected_entry() const
    {
        return static_cast<entry>(_menu_index);
    }

    void menu_controller::adjust_selected_option(
            int delta, int block_style_count, int palette_style_count, update_result& result)
    {
        if(delta == 0)
        {
            return;
        }

        switch(selected_entry())
        {
            case entry::sfx_volume:
                _sfx_volume_step = wrap_index(_sfx_volume_step + delta, volume_step_count);
                result.sfx_volume_changed = true;
                result.option_changed = true;
                break;

            case entry::music_volume:
                _music_volume_step = wrap_index(_music_volume_step + delta, volume_step_count);
                result.music_volume_changed = true;
                result.option_changed = true;
                break;

            case entry::blocks:
                _block_style = wrap_index(_block_style + delta, block_style_count);
                result.option_changed = true;
                break;

            case entry::palette:
                _palette_style = wrap_index(_palette_style + delta, palette_style_count);
                result.option_changed = true;
                break;

            case entry::assist:
                _assist_enabled = ! _assist_enabled;
                result.option_changed = true;
                break;

            case entry::start_game:
            case entry::high_scores:
            case entry::credits:
            case entry::count:
                break;

            default:
                break;
        }
    }
}
