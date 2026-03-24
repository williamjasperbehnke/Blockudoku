#ifndef BLOCKUDOKU_UI_RENDERER_H
#define BLOCKUDOKU_UI_RENDERER_H

#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_item.h"
#include "bn_regular_bg_map_cell.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_unique_ptr.h"
#include "bn_vector.h"

#include "blockudoku/gameplay_feedback.h"

namespace blockudoku
{

class game_state;
class high_scores;

class ui_renderer
{
public:
    static constexpr int block_style_count = 3;
    static constexpr int palette_style_count = 3;

    ui_renderer();

    void set_block_style(int block_style);
    void set_palette_style(int palette_style);
    void trigger_clear_feedback(int cleared_cells);

    void render(const game_state& state);
    void render_main_menu(
            const high_scores& scores, int menu_index, int sfx_volume_percent, int music_volume_percent,
            int block_style, int palette_style, bool assist_enabled);
    void render_high_scores(const high_scores& scores);
    void render_credits();
    void render_initials_entry(int score, const char initials[3], int selected_index);

private:
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
    bn::sprite_text_generator _selected_tray_generator;
    bn::vector<bn::sprite_ptr, 120> _text_sprites;
    scene_bg_type _scene_bg_type = scene_bg_type::none;
    int _block_style = 0;
    int _loaded_block_style = -1;
    int _palette_style = 0;
    int _loaded_palette_style = -1;
    gameplay_feedback _feedback;

    void build_static_bg();
    void rebuild_ui_bg();
    void set_scene_background(scene_bg_type type);

    void draw_header(const game_state& state);

    void draw_board(const game_state& state);

    void draw_tray(const game_state& state);

    void set_ui_cell(int map_x, int map_y, int tile_index);
    void clear_ui_region(int left, int top, int right, int bottom);
    void clear_ui_map();
    void commit_frame();
    void draw_title_blocks();

    [[nodiscard]] const char* block_style_label() const;
    [[nodiscard]] const char* palette_style_label() const;
};

}

#endif
