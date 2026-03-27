#ifndef BLOCKUDOKU_GAMEPLAY_FEEDBACK_H
#define BLOCKUDOKU_GAMEPLAY_FEEDBACK_H

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

namespace bn
{
    class regular_bg_ptr;
    class sprite_text_generator;
}

namespace blockudoku
{

class game_state;

class gameplay_feedback
{
public:
    void on_clear(int cleared_cells, bool full_board_clear);
    void on_scene_change(bool gameplay_scene, bn::optional<bn::regular_bg_ptr>& gameplay_bg,
                         bn::optional<bn::regular_bg_ptr>& ui_bg);
    void update_careful_condition(const game_state& state);
    void draw_messages(const game_state& state, bn::sprite_text_generator& text_generator,
                       bn::sprite_text_generator& accent_generator, bn::ivector<bn::sprite_ptr>& output_sprites);
    void apply_shake(bn::optional<bn::regular_bg_ptr>& gameplay_bg, bn::optional<bn::regular_bg_ptr>& ui_bg,
                     bn::ivector<bn::sprite_ptr>& text_sprites);
    [[nodiscard]] bool full_clear_animation_active() const;
    [[nodiscard]] int full_clear_animation_age() const;
    void advance_full_clear_animation();

private:
    int _shake_frames = 0;
    int _shake_index = 0;
    int _clear_popup_frames = 0;
    int _last_cleared_cells = 0;
    int _full_clear_animation_frames = 0;
    bool _last_full_board_clear = false;
    int _careful_popup_frames = 0;
    bool _careful_condition_previous = false;
};

}

#endif
