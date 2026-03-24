#include "blockudoku/initials_entry_controller.h"

namespace blockudoku
{
    namespace
    {
        [[nodiscard]] int wrap_index(int value, int count)
        {
            return (value % count + count) % count;
        }
    }

    void initials_entry_controller::begin()
    {
        _initials[0] = 'A';
        _initials[1] = 'A';
        _initials[2] = 'A';
        _cursor_index = 0;
    }

    void initials_entry_controller::move_cursor(int delta)
    {
        _cursor_index = wrap_index(_cursor_index + delta, initials_count);
    }

    void initials_entry_controller::adjust_selected_letter(int delta)
    {
        int next_letter = (_initials[_cursor_index] - 'A') + delta;
        next_letter = wrap_index(next_letter, 26);
        _initials[_cursor_index] = char('A' + next_letter);
    }

    bool initials_entry_controller::confirm()
    {
        if(_cursor_index < initials_count - 1)
        {
            ++_cursor_index;
            return false;
        }

        return true;
    }

    const char* initials_entry_controller::initials() const
    {
        return _initials;
    }

    int initials_entry_controller::cursor_index() const
    {
        return _cursor_index;
    }
}
