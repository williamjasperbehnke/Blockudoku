#ifndef BLOCKUDOKU_RUN_SEED_CONTROLLER_H
#define BLOCKUDOKU_RUN_SEED_CONTROLLER_H

namespace blockudoku
{

class run_seed_controller
{
public:
    static constexpr int digits_count = 8;

    void begin_manual_entry();
    void move_manual_cursor(int delta);
    void adjust_manual_digit(int delta);

    [[nodiscard]] const char* manual_digits() const;
    [[nodiscard]] int manual_cursor_index() const;
    [[nodiscard]] unsigned manual_seed() const;

    [[nodiscard]] unsigned next_auto_seed(unsigned entropy);

private:
    unsigned _seed_counter = 0xC0DE1234u;
    char _manual_digits[digits_count] = { '0', '0', '0', '0', '0', '0', '0', '1' };
    int _manual_digit_index = 0;

    static unsigned xorshift32(unsigned value);
    static unsigned normalize_seed(unsigned value);
};

}

#endif
