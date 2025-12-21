#pragma once

#include <cstdint>

class Board {
    uint64_t white = 0;
    uint64_t black = 0;
    uint64_t pawn = 0;
    uint64_t knight = 0;
    uint64_t bishop = 0;
    uint64_t rook = 0;
    uint64_t queen = 0;
    uint64_t king = 0;
    public:
    void setup_normal();
    void add_piece(uint64_t pos, char piece);
    char get_piece(uint64_t pos) const;
    void move_piece(uint64_t from, uint64_t to);
};
