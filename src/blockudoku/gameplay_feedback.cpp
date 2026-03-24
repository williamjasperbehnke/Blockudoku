#include "blockudoku/gameplay_feedback.h"

#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"

#include "blockudoku/game_state.h"

namespace blockudoku
{
    namespace
    {
        constexpr int clear_popup_duration_frames = 28;
        constexpr int careful_popup_duration_frames = 44;
        constexpr int careful_low_moves_threshold = 12;
        constexpr int careful_fade_start_frames = 16;
        constexpr int careful_fast_fade_start_frames = 8;
        constexpr int big_clear_cells_threshold = 12;
    }

    void gameplay_feedback::on_clear(int cleared_cells)
    {
        if(cleared_cells > 0)
        {
            const int frames = 4 + (cleared_cells / 2);
            _shake_frames = frames < 12 ? frames : 12;
            _clear_popup_frames = clear_popup_duration_frames;
            _last_cleared_cells = cleared_cells;
        }
    }

    void gameplay_feedback::on_scene_change(bool gameplay_scene, bn::optional<bn::regular_bg_ptr>& gameplay_bg,
                                            bn::optional<bn::regular_bg_ptr>& ui_bg)
    {
        if(gameplay_scene)
        {
            return;
        }

        _shake_frames = 0;
        _clear_popup_frames = 0;
        _last_cleared_cells = 0;
        _careful_popup_frames = 0;
        _careful_condition_previous = false;

        if(gameplay_bg)
        {
            gameplay_bg->set_position(0, 0);
        }

        if(ui_bg)
        {
            ui_bg->set_position(0, 0);
        }
    }

    void gameplay_feedback::update_careful_condition(const game_state& state)
    {
        int active_slots = 0;
        for(int slot = 0; slot < game_state::slot_count; ++slot)
        {
            if(state.slot_active(slot))
            {
                ++active_slots;
            }
        }

        const bool show_careful_condition =
                active_slots == game_state::slot_count && state.moves_available() <= careful_low_moves_threshold;

        if(show_careful_condition && ! _careful_condition_previous)
        {
            _careful_popup_frames = careful_popup_duration_frames;
        }

        _careful_condition_previous = show_careful_condition;
    }

    void gameplay_feedback::draw_messages(const game_state& state, bn::sprite_text_generator& text_generator,
                                          bn::sprite_text_generator& accent_generator,
                                          bn::ivector<bn::sprite_ptr>& output_sprites)
    {
        if(_clear_popup_frames > 0)
        {
            const int age = clear_popup_duration_frames - _clear_popup_frames;
            const int rise = age > 0 ? age / 4 : 0;
            const int wobble = (age % 6 < 3) ? -1 : 1;
            const int text_x = -40 + wobble;
            const int text_y = -2 - rise;

            if(state.combo_streak() > 1)
            {
                bn::string<20> combo_text("COMBO x");
                combo_text += bn::to_string<4>(state.combo_streak());
                accent_generator.generate(text_x + 1, text_y + 1, combo_text, output_sprites);
                text_generator.generate(text_x, text_y, combo_text, output_sprites);
            }
            else
            {
                const char* clear_text =
                        _last_cleared_cells >= big_clear_cells_threshold ? "BIG CLEAR!" : "NICE CLEAR!";
                accent_generator.generate(text_x + 1, text_y + 1, clear_text, output_sprites);
                text_generator.generate(text_x, text_y, clear_text, output_sprites);
            }

            --_clear_popup_frames;
        }

        if(_careful_popup_frames > 0)
        {
            const int age = careful_popup_duration_frames - _careful_popup_frames;
            const int rise = age > 0 ? age / 6 : 0;
            const int pulse = (age % 10 < 5) ? -1 : 1;
            const int text_x = -58 + pulse;
            const int text_y = 8 - rise;

            bool visible = true;
            if(_careful_popup_frames <= careful_fade_start_frames)
            {
                const int blink_divisor = _careful_popup_frames <= careful_fast_fade_start_frames ? 2 : 3;
                visible = (age / blink_divisor) % 2 == 0;
            }

            if(visible)
            {
                accent_generator.generate(text_x + 1, text_y + 1, "CHOOSE CAREFULLY", output_sprites);
                text_generator.generate(text_x, text_y, "CHOOSE CAREFULLY", output_sprites);
            }

            --_careful_popup_frames;
        }
    }

    void gameplay_feedback::apply_shake(bn::optional<bn::regular_bg_ptr>& gameplay_bg, bn::optional<bn::regular_bg_ptr>& ui_bg,
                                        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        if(_shake_frames > 0)
        {
            const int shake_x = (_shake_frames % 2 == 0) ? 1 : -1;
            const int shake_y = (_shake_frames % 3 == 0) ? 1 : 0;

            if(gameplay_bg)
            {
                gameplay_bg->set_position(shake_x, shake_y);
            }

            if(ui_bg)
            {
                ui_bg->set_position(shake_x, shake_y);
            }

            for(bn::sprite_ptr& sprite : text_sprites)
            {
                sprite.set_x(sprite.x() + shake_x);
                sprite.set_y(sprite.y() + shake_y);
            }

            --_shake_frames;
        }
        else
        {
            if(gameplay_bg)
            {
                gameplay_bg->set_position(0, 0);
            }

            if(ui_bg)
            {
                ui_bg->set_position(0, 0);
            }
        }
    }
}
