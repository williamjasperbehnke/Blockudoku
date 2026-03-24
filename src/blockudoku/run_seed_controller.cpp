#include "blockudoku/run_seed_controller.h"

namespace blockudoku
{
    namespace
    {
        template<int Count>
        void set_digits_from_value(unsigned value, char (&digits)[Count])
        {
            for(int index = Count - 1; index >= 0; --index)
            {
                digits[index] = char('0' + (value % 10));
                value /= 10;
            }
        }

        template<int Count>
        [[nodiscard]] unsigned value_from_digits(const char (&digits)[Count])
        {
            unsigned value = 0;

            for(char digit : digits)
            {
                value = (value * 10) + unsigned(digit - '0');
            }

            return value;
        }

        [[nodiscard]] int wrap_index(int value, int count)
        {
            return (value % count + count) % count;
        }
    }

    void run_seed_controller::begin_manual_entry()
    {
        set_digits_from_value(_seed_counter, _manual_digits);
        _manual_digit_index = 0;
    }

    void run_seed_controller::move_manual_cursor(int delta)
    {
        _manual_digit_index = wrap_index(_manual_digit_index + delta, digits_count);
    }

    void run_seed_controller::adjust_manual_digit(int delta)
    {
        int next_digit = (_manual_digits[_manual_digit_index] - '0') + delta;
        next_digit = wrap_index(next_digit, 10);
        _manual_digits[_manual_digit_index] = char('0' + next_digit);
    }

    const char* run_seed_controller::manual_digits() const
    {
        return _manual_digits;
    }

    int run_seed_controller::manual_cursor_index() const
    {
        return _manual_digit_index;
    }

    unsigned run_seed_controller::manual_seed() const
    {
        return normalize_seed(value_from_digits(_manual_digits));
    }

    unsigned run_seed_controller::next_auto_seed(unsigned entropy)
    {
        _seed_counter = xorshift32(_seed_counter);
        const unsigned mixed_seed = _seed_counter ^ (entropy << 1) ^ 0x9E3779B9u;
        return normalize_seed(mixed_seed);
    }

    unsigned run_seed_controller::xorshift32(unsigned value)
    {
        value ^= value << 13;
        value ^= value >> 17;
        value ^= value << 5;
        return value;
    }

    unsigned run_seed_controller::normalize_seed(unsigned value)
    {
        return value == 0 ? 1u : value;
    }
}
