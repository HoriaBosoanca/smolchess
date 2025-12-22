#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

enum Color {
    WHITE,
    BLACK
};
enum Piece {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};
inline std::unordered_map<int, char> piece_map = {
    {PAWN, 'P'},
    {KNIGHT, 'N'},
    {BISHOP, 'B'},
    {ROOK, 'R'},
    {QUEEN, 'Q'},
    {KING, 'K'}
};

class Board {
    uint64_t bitboard[2][6] = {};
    public:
    void setup_normal();
    char get_piece(uint64_t pos) const;
    void add_piece(uint64_t pos, int color, int piece);
    void move_piece(uint64_t from, uint64_t to);
    int move_piece_str(std::string& move);
};
