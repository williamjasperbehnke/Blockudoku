#ifndef BLOCKUDOKU_HIGH_SCORES_H
#define BLOCKUDOKU_HIGH_SCORES_H

#include "bn_array.h"

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

private:
    struct sram_data
    {
        char tag[8] = {};
        bn::array<entry, entries_count> entries;
    };

    sram_data _data;

    void load();

    void save() const;

    void set_default_entries();
};

}

#endif
