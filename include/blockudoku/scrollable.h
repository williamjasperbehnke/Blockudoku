#ifndef BLOCKUDOKU_SCROLLABLE_H
#define BLOCKUDOKU_SCROLLABLE_H

namespace blockudoku
{

class scrollable
{
public:
    void configure(int item_count, int visible_count);
    void reset();
    void scroll_up();
    void scroll_down();

    [[nodiscard]] int index() const;
    [[nodiscard]] int max_index() const;

private:
    int _index = 0;
    int _item_count = 0;
    int _visible_count = 1;
};

}

#endif
