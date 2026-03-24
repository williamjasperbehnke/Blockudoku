#ifndef BLOCKUDOKU_INITIALS_ENTRY_CONTROLLER_H
#define BLOCKUDOKU_INITIALS_ENTRY_CONTROLLER_H

namespace blockudoku
{

class initials_entry_controller
{
public:
    static constexpr int initials_count = 3;

    void begin();
    void move_cursor(int delta);
    void adjust_selected_letter(int delta);

    // Advances to the next letter and returns true when entry is complete.
    [[nodiscard]] bool confirm();

    [[nodiscard]] const char* initials() const;
    [[nodiscard]] int cursor_index() const;

private:
    char _initials[initials_count] = { 'A', 'A', 'A' };
    int _cursor_index = 0;
};

}

#endif
