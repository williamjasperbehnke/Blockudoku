#ifndef BLOCKUDOKU_MENU_CONTROLLER_H
#define BLOCKUDOKU_MENU_CONTROLLER_H

#include "blockudoku/dpad_repeater.h"
#include "blockudoku/style_types.h"

namespace blockudoku
{

class menu_controller
{
public:
    static constexpr int max_volume_step = 10;
    static constexpr int volume_step_count = max_volume_step + 1;

    enum class action
    {
        none,
        start_game,
        open_seed_entry,
        show_high_scores,
        show_achievements,
        show_credits
    };

    struct update_result
    {
        action next_action = action::none;
        bool selection_changed = false;
        bool option_changed = false;
        bool sfx_volume_changed = false;
        bool music_volume_changed = false;
    };

    [[nodiscard]] update_result update();

    [[nodiscard]] int menu_index() const;
    [[nodiscard]] int sfx_volume_step() const;
    [[nodiscard]] int music_volume_step() const;
    [[nodiscard]] block_style selected_block_style() const;
    [[nodiscard]] palette_style selected_palette_style() const;
    [[nodiscard]] bool assist_enabled() const;

private:
    enum class entry
    {
        start_game = 0,
        high_scores = 1,
        achievements = 2,
        credits = 3,
        sfx_volume = 4,
        music_volume = 5,
        blocks = 6,
        palette = 7,
        assist = 8,
        count = 9
    };

    int _menu_index = 0;
    int _sfx_volume_step = 10;
    int _music_volume_step = 5;
    block_style _block_style = block_style::classic;
    palette_style _palette_style = palette_style::classic;
    bool _assist_enabled = false;
    dpad_repeater _dpad;

    [[nodiscard]] bool selected_entry_is_option() const;
    [[nodiscard]] entry selected_entry() const;
    void adjust_selected_option(int delta, update_result& result);
};

}

#endif
