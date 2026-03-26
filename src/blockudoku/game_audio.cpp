#include "blockudoku/game_audio.h"

#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_sound_items.h"

namespace blockudoku
{
    void game_audio::set_sfx_volume(bn::fixed volume)
    {
        _sfx_volume = volume;
    }

    bn::fixed game_audio::sfx_volume() const
    {
        return _sfx_volume;
    }

    void game_audio::set_music_volume(bn::fixed volume)
    {
        _music_volume = volume;
    }

    bn::fixed game_audio::music_volume() const
    {
        return _music_volume;
    }

    void game_audio::update_music() const
    {
        if(_music_volume > 0)
        {
            if(! bn::music::playing())
            {
                bn::music_items::menu_theme.play(_music_volume);
            }
            else
            {
                bn::music::set_volume(_music_volume);
            }
        }
        else if(bn::music::playing())
        {
            bn::music::stop();
        }
    }

    void game_audio::on_event(const game_event& event) const
    {
        if(_sfx_volume <= 0)
        {
            return;
        }

        switch(event.type)
        {
            case game_event_type::hint_requested:
                bn::sound_items::reload.play(0.45 * _sfx_volume, 0.9, 0);
                break;

            case game_event_type::slot_changed:
                bn::sound_items::reload.play(0.55 * _sfx_volume);
                break;

            case game_event_type::hint_cycled:
                bn::sound_items::reload.play(0.55 * _sfx_volume, 1.35, 0);
                break;

            case game_event_type::placed:
                bn::sound_items::cure.play(0.5 * _sfx_volume);
                break;

            case game_event_type::invalid:
                bn::sound_items::alert.play(0.75 * _sfx_volume);
                break;

            case game_event_type::cleared:
                bn::sound_items::clear_chunk.play(1.0 * _sfx_volume);
                break;

            case game_event_type::game_over:
                bn::sound_items::defeat.play(0.95 * _sfx_volume);
                break;

            case game_event_type::reset:
                bn::sound_items::cure.play(0.6 * _sfx_volume);
                break;

            case game_event_type::none:
            case game_event_type::cursor_moved:
                break;

            default:
                break;
        }
    }
}
