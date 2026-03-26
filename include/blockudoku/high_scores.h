#ifndef BLOCKUDOKU_HIGH_SCORES_H
#define BLOCKUDOKU_HIGH_SCORES_H

#include "bn_array.h"
#include "blockudoku/game_state.h"

namespace blockudoku
{

class high_scores
{
public:
    struct entry
    {
        char initials[3] = { '-', '-', '-' };
        int score = 0;
        unsigned seed = 0;
    };

    static constexpr int entries_count = 5;

    high_scores();

    [[nodiscard]] const bn::array<entry, entries_count>& entries() const;

    [[nodiscard]] bool qualifies(int score) const;

    void insert(const char initials[3], int score, unsigned seed);
    [[nodiscard]] bool has_saved_game() const;
    [[nodiscard]] int saved_game_score() const;
    [[nodiscard]] unsigned saved_game_seed() const;
    void save_game_state(const game_state& state);
    [[nodiscard]] bool load_saved_game(game_state& state);
    void clear_saved_game();

private:
    struct sram_data
    {
        char tag[8] = {};
        bn::array<entry, entries_count> entries;
        bool saved_game_present = false;
        game_state::snapshot saved_game;
    };

    sram_data _data;

    void load();

    void save() const;

    void set_default_entries();
};

}

#endif
