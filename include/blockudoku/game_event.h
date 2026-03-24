#ifndef BLOCKUDOKU_GAME_EVENT_H
#define BLOCKUDOKU_GAME_EVENT_H

namespace blockudoku
{

enum class game_event_type
{
    none,
    hint_requested,
    cursor_moved,
    slot_changed,
    hint_cycled,
    placed,
    invalid,
    cleared,
    game_over,
    reset,
};

struct game_event
{
    game_event_type type = game_event_type::none;
    int cleared_cells = 0;
};

}

#endif
