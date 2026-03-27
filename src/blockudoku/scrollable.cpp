#include "blockudoku/scrollable.h"

namespace blockudoku
{
    void scrollable::configure(int item_count, int visible_count)
    {
        _item_count = item_count > 0 ? item_count : 0;
        _visible_count = visible_count > 0 ? visible_count : 1;

        if(_index > max_index())
        {
            _index = max_index();
        }
    }

    void scrollable::reset()
    {
        _index = 0;
    }

    void scrollable::scroll_up()
    {
        if(_index > 0)
        {
            --_index;
        }
    }

    void scrollable::scroll_down()
    {
        if(_index < max_index())
        {
            ++_index;
        }
    }

    int scrollable::index() const
    {
        return _index;
    }

    int scrollable::max_index() const
    {
        const int max_scroll = _item_count - _visible_count;
        return max_scroll > 0 ? max_scroll : 0;
    }
}
