#include "blockudoku/high_scores.h"

#include "bn_sram.h"

namespace blockudoku
{
    namespace
    {
        constexpr char format_tag[8] = { 'B', 'L', 'K', 'D', 'K', '2', 0, 0 };

        [[nodiscard]] bool has_valid_tag(const char tag[8])
        {
            for(int index = 0; index < int(sizeof(format_tag)); ++index)
            {
                if(tag[index] != format_tag[index])
                {
                    return false;
                }
            }

            return true;
        }
    }

    high_scores::high_scores()
    {
        load();
    }

    const bn::array<high_scores::entry, high_scores::entries_count>& high_scores::entries() const
    {
        return _data.entries;
    }

    bool high_scores::qualifies(int score) const
    {
        return score > _data.entries.back().score;
    }

    void high_scores::insert(const char initials[3], int score, unsigned seed)
    {
        entry new_entry;
        new_entry.initials[0] = initials[0];
        new_entry.initials[1] = initials[1];
        new_entry.initials[2] = initials[2];
        new_entry.score = score;
        new_entry.seed = seed;

        int insert_index = entries_count;

        for(int index = 0; index < entries_count; ++index)
        {
            if(score > _data.entries[index].score)
            {
                insert_index = index;
                break;
            }
        }

        if(insert_index >= entries_count)
        {
            return;
        }

        for(int index = entries_count - 1; index > insert_index; --index)
        {
            _data.entries[index] = _data.entries[index - 1];
        }

        _data.entries[insert_index] = new_entry;
        save();
    }

    void high_scores::load()
    {
        bn::sram::read(_data);

        if(! has_valid_tag(_data.tag))
        {
            set_default_entries();
            save();
        }
    }

    void high_scores::save() const
    {
        bn::sram::write(_data);
    }

    void high_scores::set_default_entries()
    {
        for(int index = 0; index < int(sizeof(format_tag)); ++index)
        {
            _data.tag[index] = format_tag[index];
        }

        for(entry& score_entry : _data.entries)
        {
            score_entry.initials[0] = '-';
            score_entry.initials[1] = '-';
            score_entry.initials[2] = '-';
            score_entry.score = 0;
            score_entry.seed = 0;
        }
    }
}
