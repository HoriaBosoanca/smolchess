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
    KING = 5,
    NONE = 6
};
struct Piece_and_Color {
    Piece piece;
    Color color;
};

struct Move {
    uint16_t move;
    Move();
    Move(uint8_t from, uint8_t to, Piece piece);
    void print() const;
};

class Board {
    // optimized
    uint64_t bitboard[2][6] = {};
    Move moves[2][230];
    uint8_t move_count[2] = {0, 0};
    void add_move(Move move, Color color);
    void add_continuous_move(uint8_t i, uint64_t pos, Color color, Piece piece, const uint64_t* occupied, int file_increase, int rank_increase);
    // unoptimized (just for external interaction)
    void add_piece(uint64_t pos, Color color, int piece);
    bool check_move_legality(Move move, Color color) const;
    void move_unknown(uint64_t from, uint64_t to); // use if the piece type is unknown
    public:
    void setup_normal();
    bool move_unknown_str(std::string& move, Color color);
    Piece_and_Color get_piece(uint64_t pos) const;
    void print_moves(Color color) const;
    // optimized
    uint64_t get_occupied(Color color) const;
    void generate_moves(Color color);
};
