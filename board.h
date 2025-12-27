#pragma once

#include <cstdint>
#include <string>

enum Color {
    WHITE,
    BLACK
};
constexpr char piece_map[6] = {'P','N','B','R','Q','K'};
enum Piece : uint8_t {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5
};

class Move {
    uint16_t move;
    public:
    Move();
    Move(uint8_t from, uint8_t to, Piece piece);
    void print() const;
};

class Board {
    uint64_t bitboard[2][6] = {};
    Move moves[2][256];
    uint8_t move_count[2] = {0, 0};
    public:
    // unoptimized
    void setup_normal();
    void add_piece(uint64_t pos, Color color, int piece);
    static bool parse_move_format(std::string& move);
    bool move_unknown_str(std::string& move);
    char get_piece(uint64_t pos) const;
    void print_moves(Color color) const;
    // optimized
    uint64_t get_occupied(Color color) const;
    void add_move(Move move, Color color);
    void generate_moves(Color color);
    void move_unknown(uint64_t from, uint64_t to); // use if piece type is unknown
};
