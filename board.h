#pragma once

#include <cstdint>
#include <string>

enum Color {
    WHITE,
    BLACK
};
constexpr char piece_map[7] = {'P','N','B','R','Q','K', ' '};
enum Piece : uint8_t {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE,
};

class Move {
    uint16_t move;
    uint8_t pieces;
    public:
    Move();
    Move(uint8_t from, uint8_t to, Piece from_piece, Piece to_piece);
    uint8_t get_from() const;
    uint8_t get_to() const;
    uint8_t get_from_piece() const;
    uint8_t get_to_piece() const;
    void print() const;
};

class Board {
    // optimized
    uint64_t bitboard[2][6];
    Move moves[2][230];
    uint8_t move_count[2];
    Color turn;
    void add_move(Move move);
    Piece get_piece_by_color(uint64_t pos, Color color) const;
    void add_continuous_move(uint8_t i, uint64_t pos, Piece piece, const uint64_t* occupied, int file_increase, int rank_increase);
    void make_move(Move move);
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
    bool make_move_str(std::string& move_str);
    void get_piece_and_color(uint64_t pos, Piece& piece, Color& color) const;
    void print_board() const;
    void print_moves() const;
};
