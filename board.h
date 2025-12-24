#pragma once

#include <cstdint>
#include <string>

enum Color {
    WHITE,
    BLACK
};
enum Piece : char {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5
};

class Move {
    uint64_t move;
    public:
    Move(char from, char to, Piece piece);
};

class Board {
    Move moves[2][256] = {};
    uint8_t move_count[2] = {};
    uint64_t bitboard[2][6] = {};
    public:
    // unoptimized
    void setup_normal();
    void add_piece(uint64_t pos, int color, int piece);
    static bool parse_move_format(std::string& move);
    bool move_unknown_str(std::string& move);
    char get_piece(uint64_t pos) const;
    // optimized
    uint64_t get_occupied(bool white) const;
    void add_move(Move move, bool white);
    void generate_moves(bool white);
    void move_unknown(uint64_t from, uint64_t to); // use if piece type is unknown
};
