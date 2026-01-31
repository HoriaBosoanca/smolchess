#pragma once

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
constexpr char piece_map[PIECE_COUNT+1] = {'P','N','B','R','Q','K', ' '};
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
enum MoveType : uint8_t {
    REGULAR,
    EN_PASSANT,
    QUEEN_CASTLING,
    KING_CASTLING,
    KNIGHT_PROMOTION,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
};
class Move {
    uint16_t move;
    uint8_t pieces;
    public:
    Move();
    Move(uint8_t from, uint8_t to, Piece from_piece, Piece to_piece = NONE, MoveType move_type = REGULAR);
    uint8_t from() const;
    uint8_t to() const;
    MoveType move_type() const;
    uint8_t from_piece() const;
    uint8_t to_piece() const;
    void print() const;
    std::string get_string() const;
};

class Board {
    // optimized
    uint64_t bitboard[COLOR_COUNT][PIECE_COUNT];
    // 0..15 (4 bits) for en passant (0..7 white, 8..15 black) + castling rights (4 bits) (bits 5/6 for q/k side white, 7/8 for q/k side black)
    uint8_t temp_state;
    Color turn;
    void clear_en_passant();
    [[nodiscard]] Piece get_piece(uint8_t pos, bool color) const;
    [[nodiscard]] uint64_t get_occupied(Color color) const;
    void add_en_passant(uint8_t i, Color color);
    [[nodiscard]] std::optional<uint8_t> get_nearby_en_passant(uint8_t i, Color color) const;
    [[nodiscard]] std::optional<uint8_t> get_castle_move(Color color, bool queen_side) const;
    [[nodiscard]] bool is_in_check(Color color) const;
    int generate_moves(Move* moves) const;
    public:
    int generate_legal_moves(Move* legal_moves);
    void make_move(Move move);
    GameStatus game_over();
    [[nodiscard]] int eval() const;
    // user input / initialization
    private:
    void setup_normal();
    void add_piece(uint64_t pos, Color color, int piece);
    public:
    Board();
    explicit Board(const std::optional<std::string>& fen);
    [[nodiscard]] Color get_turn() const { return turn; }
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
    i += static_cast<uint8_t>(file_cnt);
    i += static_cast<uint8_t>(rank_cnt * 8);
    return i;
}