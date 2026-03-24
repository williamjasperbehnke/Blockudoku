#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_core.h"

#include "blockudoku/game_app.h"

int main()
{
    bn::core::init();
    bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

    blockudoku::game_app app;

    while(true)
    {
        app.update();
        bn::core::update();
    }
}
