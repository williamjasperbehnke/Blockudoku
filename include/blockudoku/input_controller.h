#ifndef BLOCKUDOKU_INPUT_CONTROLLER_H
#define BLOCKUDOKU_INPUT_CONTROLLER_H

#include "blockudoku/game_event.h"

namespace blockudoku
{

class game_state;

class input_controller
{
public:
    [[nodiscard]] game_event update(game_state& state) const;
};

}

#endif
