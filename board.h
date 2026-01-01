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
    Color turn;
    Piece get_piece(uint64_t pos, Color color) const;
    void add_continuous_move(uint8_t i, uint64_t pos, Piece piece, const uint64_t* occupied, int file_increase, int rank_increase, Move* moves, int& c) const;
    uint64_t get_occupied(Color color) const;
    public:
    int generate_moves(Move* moves) const;
    void make_move(Move move);

    // user input / initialization
    private:
    void setup_normal();
    void add_piece(uint64_t pos, Color color, int piece);
    public:
    Board();
    Color get_turn() const;
    void print_board() const;
    void print_moves() const;
};

inline int rank(const uint8_t i) {
    return i/8+1;
}
inline char file(const uint8_t i) {
    return (char)(i%8+'a');
}
inline uint8_t offset_idx(uint8_t i, const int file_cnt, const int rank_cnt) {
    i += file_cnt;
    i += rank_cnt*8;
    return i;
}
inline uint64_t offset_pos(uint64_t pos, const int file_cnt, const int rank_cnt) {
    if (file_cnt < 0) {
        pos >>= (-file_cnt);
    } else {
        pos <<= file_cnt;
    }
    if (rank_cnt < 0) {
        pos >>= (-rank_cnt*8);
    } else {
        pos <<= rank_cnt*8;
    }
    return pos;
}