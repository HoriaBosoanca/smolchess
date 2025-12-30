#pragma once

#include <cstdint>
#include <string>

enum Color {
    WHITE,
    BLACK
};
constexpr char piece_map[6] = {'P','N','B','R','Q','K'};
enum Piece : uint8_t {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE,
};

struct Move {
    uint16_t move;
    Move();
    Move(uint8_t from, uint8_t to, Piece piece);
    void print() const;
};

class Board {
    // optimized
    uint64_t bitboard[2][6];
    Move moves[2][230];
    uint8_t move_count[2];
    Color turn;
    void add_move(Move move);
    void add_continuous_move(uint8_t i, uint64_t pos, Piece piece, const uint64_t* occupied, int file_increase, int rank_increase);
    void make_move(uint64_t from, uint64_t to, Piece piece);
    uint64_t get_occupied(Color color) const;
    public:
    void generate_moves();

    // user input / initialization
    private:
    void setup_normal();
    void add_piece(uint64_t pos, Color color, int piece);
    bool check_move_legality(Move move) const;
    public:
    Board();
    Color get_turn() const;
    bool make_move_str(std::string& move);
    void get_piece(uint64_t pos, Piece& piece, Color& color) const;
    void print_board() const;
    void print_moves() const;
};
