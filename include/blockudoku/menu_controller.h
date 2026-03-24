#ifndef BLOCKUDOKU_MENU_CONTROLLER_H
#define BLOCKUDOKU_MENU_CONTROLLER_H

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

    [[nodiscard]] update_result update(int block_style_count, int palette_style_count);

    [[nodiscard]] int menu_index() const;
    [[nodiscard]] int sfx_volume_step() const;
    [[nodiscard]] int music_volume_step() const;
    [[nodiscard]] int block_style() const;
    [[nodiscard]] int palette_style() const;
    [[nodiscard]] bool assist_enabled() const;

private:
    enum class entry
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

    int _menu_index = 0;
    int _sfx_volume_step = 10;
    int _music_volume_step = 10;
    int _block_style = 0;
    int _palette_style = 0;
    bool _assist_enabled = false;

    [[nodiscard]] bool selected_entry_is_option() const;
    [[nodiscard]] entry selected_entry() const;
    void adjust_selected_option(int delta, int block_style_count, int palette_style_count, update_result& result);
};

}

#endif
