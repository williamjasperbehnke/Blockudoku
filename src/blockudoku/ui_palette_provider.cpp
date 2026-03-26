#include "blockudoku/ui_palette_provider.h"

#include "bn_array.h"
#include "bn_bg_palette_item.h"
#include "bn_bg_palette_ptr.h"
#include "bn_bg_palette_items_palette_classic.h"
#include "bn_bg_palette_items_palette_colorblind.h"
#include "bn_bg_palette_items_palette_forest.h"
#include "bn_bg_palette_items_palette_gameboy.h"
#include "bn_bg_palette_items_palette_grayscale.h"
#include "bn_bg_palette_items_palette_mint.h"
#include "bn_bg_palette_items_palette_ocean.h"
#include "bn_bg_palette_items_palette_sunrise.h"
#include "bn_bpp_mode.h"
#include "bn_color.h"
#include "bn_sprite_palette_item.h"

namespace blockudoku::ui_palette_provider
{
    namespace
    {
        constexpr bn::array<bn::color, 16> text_palette_classic = {
            bn::color(0, 31, 0), bn::color(29, 29, 29), bn::color(2, 2, 2), bn::color(2, 2, 2),
            bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0),
            bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0),
            bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0)
        };
        constexpr bn::array<bn::color, 16> text_palette_sunrise = {
            bn::color(0, 31, 0), bn::color(30, 28, 25), bn::color(15, 8, 4), bn::color(15, 8, 4),
            bn::color(14, 6, 2), bn::color(14, 6, 2), bn::color(14, 6, 2), bn::color(14, 6, 2),
            bn::color(14, 6, 2), bn::color(14, 6, 2), bn::color(14, 6, 2), bn::color(14, 6, 2),
            bn::color(14, 6, 2), bn::color(14, 6, 2), bn::color(14, 6, 2), bn::color(14, 6, 2)
        };
        constexpr bn::array<bn::color, 16> text_palette_mint = {
            bn::color(0, 31, 0), bn::color(29, 30, 29), bn::color(5, 10, 8), bn::color(5, 10, 8),
            bn::color(3, 10, 7), bn::color(3, 10, 7), bn::color(3, 10, 7), bn::color(3, 10, 7),
            bn::color(3, 10, 7), bn::color(3, 10, 7), bn::color(3, 10, 7), bn::color(3, 10, 7),
            bn::color(3, 10, 7), bn::color(3, 10, 7), bn::color(3, 10, 7), bn::color(3, 10, 7)
        };
        constexpr bn::array<bn::color, 16> text_palette_ocean = {
            bn::color(0, 31, 0), bn::color(29, 30, 30), bn::color(4, 7, 11), bn::color(4, 7, 11),
            bn::color(2, 5, 12), bn::color(2, 5, 12), bn::color(2, 5, 12), bn::color(2, 5, 12),
            bn::color(2, 5, 12), bn::color(2, 5, 12), bn::color(2, 5, 12), bn::color(2, 5, 12),
            bn::color(2, 5, 12), bn::color(2, 5, 12), bn::color(2, 5, 12), bn::color(2, 5, 12)
        };
        constexpr bn::array<bn::color, 16> text_palette_forest = {
            bn::color(0, 31, 0), bn::color(28, 30, 27), bn::color(5, 9, 5), bn::color(5, 9, 5),
            bn::color(4, 8, 3), bn::color(4, 8, 3), bn::color(4, 8, 3), bn::color(4, 8, 3),
            bn::color(4, 8, 3), bn::color(4, 8, 3), bn::color(4, 8, 3), bn::color(4, 8, 3),
            bn::color(4, 8, 3), bn::color(4, 8, 3), bn::color(4, 8, 3), bn::color(4, 8, 3)
        };
        constexpr bn::array<bn::color, 16> text_palette_grayscale = {
            bn::color(0, 31, 0), bn::color(29, 29, 29), bn::color(7, 7, 7), bn::color(7, 7, 7),
            bn::color(6, 6, 6), bn::color(6, 6, 6), bn::color(6, 6, 6), bn::color(6, 6, 6),
            bn::color(6, 6, 6), bn::color(6, 6, 6), bn::color(6, 6, 6), bn::color(6, 6, 6),
            bn::color(6, 6, 6), bn::color(6, 6, 6), bn::color(6, 6, 6), bn::color(6, 6, 6)
        };
        constexpr bn::array<bn::color, 16> text_palette_colorblind = {
            bn::color(0, 31, 0), bn::color(30, 30, 29), bn::color(6, 6, 12), bn::color(6, 6, 12),
            bn::color(5, 5, 12), bn::color(5, 5, 12), bn::color(5, 5, 12), bn::color(5, 5, 12),
            bn::color(5, 5, 12), bn::color(5, 5, 12), bn::color(5, 5, 12), bn::color(5, 5, 12),
            bn::color(5, 5, 12), bn::color(5, 5, 12), bn::color(5, 5, 12), bn::color(5, 5, 12)
        };
        constexpr bn::array<bn::color, 16> text_palette_gameboy = {
            bn::color(0, 31, 0), bn::color(21, 26, 18), bn::color(4, 8, 4), bn::color(4, 8, 4),
            bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2),
            bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2),
            bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2)
        };

        constexpr bn::array<bn::color, 16> accent_palette_classic = {
            bn::color(0, 31, 0), bn::color(17, 24, 29), bn::color(4, 7, 17), bn::color(4, 7, 17),
            bn::color(2, 5, 17), bn::color(2, 5, 17), bn::color(2, 5, 17), bn::color(2, 5, 17),
            bn::color(2, 5, 17), bn::color(2, 5, 17), bn::color(2, 5, 17), bn::color(2, 5, 17),
            bn::color(2, 5, 17), bn::color(2, 5, 17), bn::color(2, 5, 17), bn::color(2, 5, 17)
        };
        constexpr bn::array<bn::color, 16> accent_palette_sunrise = {
            bn::color(0, 31, 0), bn::color(30, 23, 16), bn::color(13, 6, 4), bn::color(13, 6, 4),
            bn::color(12, 4, 2), bn::color(12, 4, 2), bn::color(12, 4, 2), bn::color(12, 4, 2),
            bn::color(12, 4, 2), bn::color(12, 4, 2), bn::color(12, 4, 2), bn::color(12, 4, 2),
            bn::color(12, 4, 2), bn::color(12, 4, 2), bn::color(12, 4, 2), bn::color(12, 4, 2)
        };
        constexpr bn::array<bn::color, 16> accent_palette_mint = {
            bn::color(0, 31, 0), bn::color(19, 30, 24), bn::color(4, 11, 8), bn::color(4, 11, 8),
            bn::color(2, 10, 6), bn::color(2, 10, 6), bn::color(2, 10, 6), bn::color(2, 10, 6),
            bn::color(2, 10, 6), bn::color(2, 10, 6), bn::color(2, 10, 6), bn::color(2, 10, 6),
            bn::color(2, 10, 6), bn::color(2, 10, 6), bn::color(2, 10, 6), bn::color(2, 10, 6)
        };
        constexpr bn::array<bn::color, 16> accent_palette_ocean = {
            bn::color(0, 31, 0), bn::color(15, 24, 29), bn::color(4, 6, 12), bn::color(4, 6, 12),
            bn::color(2, 4, 12), bn::color(2, 4, 12), bn::color(2, 4, 12), bn::color(2, 4, 12),
            bn::color(2, 4, 12), bn::color(2, 4, 12), bn::color(2, 4, 12), bn::color(2, 4, 12),
            bn::color(2, 4, 12), bn::color(2, 4, 12), bn::color(2, 4, 12), bn::color(2, 4, 12)
        };
        constexpr bn::array<bn::color, 16> accent_palette_forest = {
            bn::color(0, 31, 0), bn::color(19, 27, 15), bn::color(5, 8, 4), bn::color(5, 8, 4),
            bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2),
            bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2),
            bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2), bn::color(3, 7, 2)
        };
        constexpr bn::array<bn::color, 16> accent_palette_grayscale = {
            bn::color(0, 31, 0), bn::color(22, 22, 22), bn::color(4, 4, 4), bn::color(4, 4, 4),
            bn::color(3, 3, 3), bn::color(3, 3, 3), bn::color(3, 3, 3), bn::color(3, 3, 3),
            bn::color(3, 3, 3), bn::color(3, 3, 3), bn::color(3, 3, 3), bn::color(3, 3, 3),
            bn::color(3, 3, 3), bn::color(3, 3, 3), bn::color(3, 3, 3), bn::color(3, 3, 3)
        };
        constexpr bn::array<bn::color, 16> accent_palette_colorblind = {
            bn::color(0, 31, 0), bn::color(30, 23, 14), bn::color(6, 6, 12), bn::color(6, 6, 12),
            bn::color(4, 4, 12), bn::color(4, 4, 12), bn::color(4, 4, 12), bn::color(4, 4, 12),
            bn::color(4, 4, 12), bn::color(4, 4, 12), bn::color(4, 4, 12), bn::color(4, 4, 12),
            bn::color(4, 4, 12), bn::color(4, 4, 12), bn::color(4, 4, 12), bn::color(4, 4, 12)
        };
        constexpr bn::array<bn::color, 16> accent_palette_gameboy = {
            bn::color(0, 31, 0), bn::color(17, 23, 13), bn::color(4, 6, 3), bn::color(4, 6, 3),
            bn::color(2, 5, 1), bn::color(2, 5, 1), bn::color(2, 5, 1), bn::color(2, 5, 1),
            bn::color(2, 5, 1), bn::color(2, 5, 1), bn::color(2, 5, 1), bn::color(2, 5, 1),
            bn::color(2, 5, 1), bn::color(2, 5, 1), bn::color(2, 5, 1), bn::color(2, 5, 1)
        };

        constexpr bn::sprite_palette_item text_palette_item_classic(text_palette_classic, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item text_palette_item_sunrise(text_palette_sunrise, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item text_palette_item_mint(text_palette_mint, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item text_palette_item_ocean(text_palette_ocean, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item text_palette_item_forest(text_palette_forest, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item text_palette_item_grayscale(text_palette_grayscale, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item text_palette_item_colorblind(text_palette_colorblind, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item text_palette_item_gameboy(text_palette_gameboy, bn::bpp_mode::BPP_4);

        constexpr bn::sprite_palette_item accent_palette_item_classic(accent_palette_classic, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item accent_palette_item_sunrise(accent_palette_sunrise, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item accent_palette_item_mint(accent_palette_mint, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item accent_palette_item_ocean(accent_palette_ocean, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item accent_palette_item_forest(accent_palette_forest, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item accent_palette_item_grayscale(accent_palette_grayscale, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item accent_palette_item_colorblind(accent_palette_colorblind, bn::bpp_mode::BPP_4);
        constexpr bn::sprite_palette_item accent_palette_item_gameboy(accent_palette_gameboy, bn::bpp_mode::BPP_4);
    }

    const bn::bg_palette_item& background_palette_item(palette_style style)
    {
        switch(style)
        {
            case palette_style::sunrise:
                return bn::bg_palette_items::palette_sunrise;
            case palette_style::mint:
                return bn::bg_palette_items::palette_mint;
            case palette_style::ocean:
                return bn::bg_palette_items::palette_ocean;
            case palette_style::forest:
                return bn::bg_palette_items::palette_forest;
            case palette_style::grayscale:
                return bn::bg_palette_items::palette_grayscale;
            case palette_style::colorblind:
                return bn::bg_palette_items::palette_colorblind;
            case palette_style::gameboy:
                return bn::bg_palette_items::palette_gameboy;
            case palette_style::classic:
            case palette_style::count:
            default:
                return bn::bg_palette_items::palette_classic;
        }
    }

    bn::bg_palette_ptr create_background_palette(palette_style style)
    {
        return background_palette_item(style).create_palette();
    }

    const bn::sprite_palette_item& text_palette_item(palette_style style)
    {
        switch(style)
        {
            case palette_style::sunrise:
                return text_palette_item_sunrise;
            case palette_style::mint:
                return text_palette_item_mint;
            case palette_style::ocean:
                return text_palette_item_ocean;
            case palette_style::forest:
                return text_palette_item_forest;
            case palette_style::grayscale:
                return text_palette_item_grayscale;
            case palette_style::colorblind:
                return text_palette_item_colorblind;
            case palette_style::gameboy:
                return text_palette_item_gameboy;
            case palette_style::classic:
            case palette_style::count:
            default:
                return text_palette_item_classic;
        }
    }

    const bn::sprite_palette_item& accent_palette_item(palette_style style)
    {
        switch(style)
        {
            case palette_style::sunrise:
                return accent_palette_item_sunrise;
            case palette_style::mint:
                return accent_palette_item_mint;
            case palette_style::ocean:
                return accent_palette_item_ocean;
            case palette_style::forest:
                return accent_palette_item_forest;
            case palette_style::grayscale:
                return accent_palette_item_grayscale;
            case palette_style::colorblind:
                return accent_palette_item_colorblind;
            case palette_style::gameboy:
                return accent_palette_item_gameboy;
            case palette_style::classic:
            case palette_style::count:
            default:
                return accent_palette_item_classic;
        }
    }
}
