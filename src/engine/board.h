#pragma once

#include "move.h"
#include <cstdint>
#include <optional>
#include <string>

#define Color bool
#define WHITE false
#define BLACK true
#define PIECE_COUNT 6
#define BOARD_SIZE 64
#define COLOR_COUNT 2
#define MAX_MOVES 230

#define EN_PASSANT_DATA_MASK 0b00001111
#define CASTLING_DATA_MASK 0b11110000

enum GameStatus : uint8_t {
    Ongoing,
    WhiteWin,
    BlackWin,
    Draw,
};

inline int rank(const uint8_t i) {
    return i/8+1;
}
inline int file_int(const uint8_t i) {
    return i % 8;
}
inline char file_char(const uint8_t i) {
    return static_cast<char>(file_int(i) + 'a');
}
inline uint8_t offset_idx(uint8_t i, const int file_cnt, const int rank_cnt) {
    i += static_cast<uint8_t>(file_cnt);
    i += static_cast<uint8_t>(rank_cnt * 8);
    return i;
}

class Board {
    uint64_t bitboard[COLOR_COUNT][PIECE_COUNT];
    /*
    values 0..7 (2 bits) for white en passant, values 8..15 (2 bits) for black en passant => 4 bits
    obviously there can only be one en passant on the board at a time
    castling rights (4 bits) (bits 5/6 for q/k side white, 7/8 for q/k side black)
    */
    uint8_t special_data;
    bool is_any_en_passant;
    Color turn_color;
    public: // this func
    explicit Board(const std::optional<std::string>& fen);
    private: // calls these
    void setup_normal();
    void add_piece(uint64_t pos, Color color, int piece);

    public: // this func
    int generate_legal_moves(Move* legal_moves);
    private: // calls these
    int generate_moves(Move* moves) const;
    uint64_t get_occupied(Color color) const;
    Piece get_piece(uint8_t pos, bool color) const;
    std::optional<uint8_t> get_nearby_en_passant(uint8_t i, Color color) const;
    std::optional<uint8_t> get_castle_move(Color color, bool queen_side) const;
    // make_move()
    bool is_in_check(Color color) const;

    public: // this func
    void make_move(Move move);
    private: // calls these
    void clear_all_en_passant_availability();
    void add_en_passant_availability(uint8_t i, Color color);

    public:
    GameStatus game_over();
    Color get_turn() const { return turn_color; }
    int eval() const;
};