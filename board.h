#pragma once

#include <cstdint>

#define Color bool
#define WHITE false
#define BLACK true
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
enum GameStatus : uint8_t {
    Ongoing,
    WhiteWin,
    BlackWin,
    Draw,
};
class Move {
    uint16_t move;
    uint8_t pieces;
    public:
    Move();
    Move(uint8_t from, uint8_t to, Piece from_piece, Piece to_piece);
    uint8_t from() const;
    uint8_t to() const;
    uint8_t from_piece() const;
    uint8_t to_piece() const;
    void print() const;
};

class Board {
    // optimized
    uint64_t bitboard[2][6];
    Color turn;
    Piece get_piece(uint8_t pos, bool color) const;
    uint64_t get_occupied(Color color) const;
    bool is_in_check(Color color) const;
    int generate_moves(Move* moves) const;
    void undo_move(Move move);
    public:
    int generate_legal_moves(Move* legal_moves);
    void make_move(Move move);
    GameStatus game_over();
    // user input / initialization
    private:
    void setup_normal();
    void add_piece(uint64_t pos, bool color, int piece);
    public:
    Board();
    Color get_turn() const;
    void print_board() const;
    void print_moves();
};

inline int rank(const uint8_t i) {
    return i/8+1;
}
inline char file(const uint8_t i) {
    return static_cast<char>(i % 8 + 'a');
}
inline uint8_t offset_idx(uint8_t i, const int file_cnt, const int rank_cnt) {
    i += file_cnt;
    i += rank_cnt*8;
    return i;
}