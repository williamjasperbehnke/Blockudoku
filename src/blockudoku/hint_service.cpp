#include "blockudoku/hint_service.h"

#include "blockudoku/game_state.h"

namespace blockudoku
{
    void hint_service::reset()
    {
        _task.cancel();
        _manual_pending = false;
    }

    void hint_service::request_manual(const game_state& state)
    {
        _task.begin(state);
        _manual_pending = true;
    }

    void hint_service::cancel_manual()
    {
        reset();
    }

    void hint_service::update_manual(game_state& state, game_event& in_out_event)
    {
        if(! _manual_pending || ! _task.active())
        {
            return;
        }

        _task.step(_step_budget);
        const bool hint_applied = try_apply_finished_hint(state, in_out_event);
        (void) hint_applied;
    }

    game_event hint_service::run_assist_step(game_state& state)
    {
        if(state.game_over())
        {
            return { game_event_type::none, 0 };
        }

        if(! _task.active())
        {
            _task.begin(state);
            return { game_event_type::none, 0 };
        }

        _task.step(_step_budget);
        if(! _task.finished())
        {
            return { game_event_type::none, 0 };
        }

        game_event event = { game_event_type::none, 0 };
        if(! try_apply_finished_hint(state, event))
        {
            return { game_event_type::none, 0 };
        }

        return state.try_place_selected();
    }

    bool hint_service::try_apply_finished_hint(game_state& state, game_event& out_event)
    {
        if(! _task.finished())
        {
            return false;
        }

        const hint_move best_move = _task.best_move();
        reset();

        if(! best_move.valid ||
           ! state.apply_hint_move(best_move.slot_index, best_move.base_x, best_move.base_y))
        {
            return false;
        }

        out_event = { game_event_type::slot_changed, 0 };
        return true;
    }
}
