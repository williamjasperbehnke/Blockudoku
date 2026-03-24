#ifndef BLOCKUDOKU_GAME_AUDIO_H
#define BLOCKUDOKU_GAME_AUDIO_H

#include "bn_fixed.h"

#include "blockudoku/game_event.h"

namespace blockudoku
{

class game_audio
{
public:
    void set_sfx_volume(bn::fixed volume);
    [[nodiscard]] bn::fixed sfx_volume() const;
    void set_music_volume(bn::fixed volume);
    [[nodiscard]] bn::fixed music_volume() const;
    void update_music() const;
    void on_event(const game_event& event) const;

private:
    bn::fixed _sfx_volume = 1;
    bn::fixed _music_volume = 1;
};

}

#endif
