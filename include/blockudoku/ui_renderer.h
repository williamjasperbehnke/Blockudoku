#ifndef BLOCKUDOKU_UI_RENDERER_H
#define BLOCKUDOKU_UI_RENDERER_H

#include "bn_optional.h"
#include "bn_bg_palette_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_item.h"
#include "bn_regular_bg_map_cell.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_unique_ptr.h"
#include "bn_vector.h"

#include "blockudoku/gameplay_feedback.h"
#include "blockudoku/style_types.h"

namespace blockudoku
{

class game_state;
class high_scores;
class gameplay_screen_renderer;
class menu_screen_renderer;
class info_screen_renderer;

class ui_renderer
{
public:
    ui_renderer();

    void set_block_style(block_style style);
    void set_palette_style(palette_style style);
    void trigger_clear_feedback(int cleared_cells);

    void render(const game_state& state);
    void render_main_menu(
            const high_scores& scores, int menu_index, int sfx_volume_percent, int music_volume_percent,
            block_style style, palette_style palette, bool assist_enabled);
    void render_high_scores(const high_scores& scores);
    void render_credits();
    void render_seed_entry(const char seed_digits[8], int selected_index);
    void render_initials_entry(int score, const char initials[3], int selected_index);
    void render_resume_prompt(bool continue_selected, int saved_score, unsigned saved_seed);

private:
    friend class gameplay_screen_renderer;
    friend class menu_screen_renderer;
    friend class info_screen_renderer;

    enum class scene_bg_type
    {
        none,
        title,
        info,
        gameplay
    };

    struct ui_bg_map
    {
        static constexpr int columns = 32;
        static constexpr int rows = 32;

        alignas(int) bn::regular_bg_map_cell cells[columns * rows];
        bn::regular_bg_map_item map_item;

        ui_bg_map();
    };

    bn::unique_ptr<ui_bg_map> _ui_bg_map;
    bn::optional<bn::regular_bg_ptr> _title_bg;
    bn::optional<bn::regular_bg_ptr> _info_bg;
    bn::optional<bn::regular_bg_ptr> _gameplay_bg;
    bn::optional<bn::regular_bg_ptr> _ui_bg;
    bn::optional<bn::regular_bg_map_ptr> _ui_bg_map_ptr;
    bn::sprite_text_generator _text_generator;
    bn::sprite_text_generator _accent_text_generator;
    bn::vector<bn::sprite_ptr, 120> _text_sprites;
    scene_bg_type _scene_bg_type = scene_bg_type::none;
    block_style _block_style = block_style::classic;
    block_style _loaded_block_style = block_style::count;
    palette_style _palette_style = palette_style::classic;
    palette_style _loaded_palette_style = palette_style::count;
    gameplay_feedback _feedback;

    void build_static_bg();
    void rebuild_ui_bg();
    void apply_background_palette();
    void apply_text_palette();
    void set_scene_background(scene_bg_type type);

    void set_ui_cell(int map_x, int map_y, int tile_index);
    void clear_ui_region(int left, int top, int right, int bottom);
    void clear_ui_map();
    void commit_frame();

    [[nodiscard]] const char* block_style_label() const;
    [[nodiscard]] const char* palette_style_label() const;
};

}

#endif
