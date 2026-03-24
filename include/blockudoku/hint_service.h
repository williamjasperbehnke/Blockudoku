#ifndef BLOCKUDOKU_HINT_SERVICE_H
#define BLOCKUDOKU_HINT_SERVICE_H

#include "blockudoku/game_event.h"
#include "blockudoku/hint_solver.h"

namespace blockudoku
{

class game_state;

class hint_service
{
public:
    void reset();
    void request_manual(const game_state& state);
    void cancel_manual();
    void update_manual(game_state& state, game_event& in_out_event);
    [[nodiscard]] game_event run_assist_step(game_state& state);

private:
    static constexpr int default_step_budget = 32;

    [[nodiscard]] bool try_apply_finished_hint(game_state& state, game_event& out_event);

    hint_search_task _task;
    bool _manual_pending = false;
    int _step_budget = default_step_budget;
};

}

#endif
