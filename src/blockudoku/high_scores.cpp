#include "blockudoku/high_scores.h"

#include "bn_sram.h"

namespace blockudoku
{
    namespace
    {
        constexpr char format_tag_v2[8] = { 'B', 'L', 'K', 'D', 'K', '2', 0, 0 };
        constexpr char format_tag_v3[8] = { 'B', 'L', 'K', 'D', 'K', '3', 0, 0 };
        constexpr char format_tag_v4[8] = { 'B', 'L', 'K', 'D', 'K', '4', 0, 0 };

        struct old_sram_data_v2
        {
            char tag[8] = {};
            bn::array<high_scores::entry, high_scores::entries_count> entries;
        };

        struct old_sram_data_v3
        {
            char tag[8] = {};
            bn::array<high_scores::entry, high_scores::entries_count> entries;
            bool saved_game_present = false;
            game_state::snapshot saved_game;
        };

        [[nodiscard]] bool has_tag(const char tag[8], const char expected_tag[8])
        {
            for(int index = 0; index < int(sizeof(format_tag_v4)); ++index)
            {
                if(tag[index] != expected_tag[index])
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

    bool high_scores::has_saved_game() const
    {
        return _data.saved_game_present;
    }

    int high_scores::saved_game_score() const
    {
        return _data.saved_game_present ? _data.saved_game.score : 0;
    }

    unsigned high_scores::saved_game_seed() const
    {
        return _data.saved_game_present ? _data.saved_game.run_seed : 0;
    }

    void high_scores::save_game_state(const game_state& state)
    {
        _data.saved_game = state.make_snapshot();
        _data.saved_game_present = true;
        save();
    }

    bool high_scores::load_saved_game(game_state& state)
    {
        if(! _data.saved_game_present)
        {
            return false;
        }

        if(state.restore_snapshot(_data.saved_game))
        {
            return true;
        }

        clear_saved_game();
        return false;
    }

    void high_scores::clear_saved_game()
    {
        _data.saved_game_present = false;
        _data.saved_game = {};
        save();
    }

    bool high_scores::achievement_unlocked(int index) const
    {
        if(index < 0 || index >= achievements_count)
        {
            return false;
        }

        return _data.achievement_unlocked[index];
    }

    bool high_scores::unlock_achievement(achievement_id id)
    {
        const int index = int(id);
        if(index < 0 || index >= achievements_count)
        {
            return false;
        }

        if(_data.achievement_unlocked[index])
        {
            return false;
        }

        _data.achievement_unlocked[index] = true;
        save();
        return true;
    }

    int high_scores::unlocked_achievements_count() const
    {
        int result = 0;
        for(bool unlocked : _data.achievement_unlocked)
        {
            if(unlocked)
            {
                ++result;
            }
        }
        return result;
    }

    const char* high_scores::achievement_name(int index)
    {
        switch(index)
        {
            case int(achievement_id::first_move):
                return "FIRST MOVE";
            case int(achievement_id::line_clear):
                return "LINE CLEAR";
            case int(achievement_id::big_clear):
                return "BIG CLEAR";
            case int(achievement_id::full_clear):
                return "FULL CLEAR";
            case int(achievement_id::combo_3):
                return "COMBO 3";
            case int(achievement_id::combo_4):
                return "COMBO 4";
            case int(achievement_id::combo_5):
                return "COMBO 5";
            case int(achievement_id::score_1000):
                return "SCORE 1000";
            case int(achievement_id::score_2000):
                return "SCORE 2000";
            case int(achievement_id::score_3000):
                return "SCORE 3000";
            default:
                return "UNKNOWN";
        }
    }

    void high_scores::load()
    {
        bn::sram::read(_data);


        if(has_tag(_data.tag, format_tag_v4))
        {
            return;
        }

        if(has_tag(_data.tag, format_tag_v3))
        {
            old_sram_data_v3 previous_data;
            bn::sram::read(previous_data);
            _data.entries = previous_data.entries;
            for(int index = 0; index < achievements_count; ++index)
            {
                _data.achievement_unlocked[index] = false;
            }
            for(int index = 0; index < int(sizeof(format_tag_v4)); ++index)
            {
                _data.tag[index] = format_tag_v4[index];
            }
            _data.saved_game_present = previous_data.saved_game_present;
            _data.saved_game = previous_data.saved_game;
            save();
            return;
        }

        if(has_tag(_data.tag, format_tag_v2))
        {
            old_sram_data_v2 previous_data;
            bn::sram::read(previous_data);
            _data.entries = previous_data.entries;
            for(int index = 0; index < achievements_count; ++index)
            {
                _data.achievement_unlocked[index] = false;
            }
            for(int index = 0; index < int(sizeof(format_tag_v4)); ++index)
            {
                _data.tag[index] = format_tag_v4[index];
            }
            _data.saved_game_present = false;
            _data.saved_game = {};
            save();
            return;
        }

        set_default_entries();
        save();
    }

    void high_scores::save() const
    {
        bn::sram::write(_data);
    }

    void high_scores::set_default_entries()
    {
        for(int index = 0; index < int(sizeof(format_tag_v4)); ++index)
        {
            _data.tag[index] = format_tag_v4[index];
        }

        for(entry& score_entry : _data.entries)
        {
            score_entry.initials[0] = '-';
            score_entry.initials[1] = '-';
            score_entry.initials[2] = '-';
            score_entry.score = 0;
            score_entry.seed = 0;
        }

        for(int index = 0; index < achievements_count; ++index)
        {
            _data.achievement_unlocked[index] = false;
        }

        _data.saved_game_present = false;
        _data.saved_game = {};
    }
}
