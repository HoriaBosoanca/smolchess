#include <cstdint>
#include <iostream>
#include "board.h"

// optimized

Move::Move() : move(0), pieces(0) {}

Move::Move(const uint8_t from, const uint8_t to, const Piece from_piece, const Piece to_piece) {
    move = from | (to << 6); // 16 - 6x2 = 4 bits left
    pieces = from_piece | (to_piece << 3); // 8 - 3x2 = 2 bits left
}

uint8_t Move::from() const {
    return move & 0b111111;
}

uint8_t Move::to() const {
    return (move >> 6) & 0b111111;
}

uint8_t Move::from_piece() const {
    return pieces & 0b111;
}

uint8_t Move::to_piece() const {
    return (pieces >> 3) & 0b111;
}

void Move::print() const {
    std::cout << piece_map[from_piece()] << " " << file(from()) << rank(from()) << file(to()) << rank(to()) << " " << piece_map[to_piece()] << " ";
}

Piece Board::get_piece(const uint8_t pos, const Color color) const {
    const uint64_t pos64 = (1ULL << pos);
    for (int p = PAWN; p <= KING; p++)
        if (bitboard[color][p] & pos64)
            return static_cast<Piece>(p);
    return NONE;
}

void Board::make_move(const Move move) {
    const uint64_t from = 1ULL << move.from(), to = 1ULL << move.to();
    const uint8_t from_piece = move.from_piece(), to_piece = move.to_piece();
    bitboard[turn][from_piece] = (bitboard[turn][from_piece] & ~from) | to;
    if (to_piece != NONE) {
        bitboard[!turn][to_piece] = bitboard[!turn][to_piece] & ~to;
    }
    turn = !turn;
}

void Board::undo_move(const Move move) {
    const uint64_t from = 1ULL << move.from(), to = 1ULL << move.to();
    const uint8_t from_piece = move.from_piece(), to_piece = move.to_piece();
    turn = !turn;
    // erase your piece from 'to' square
    bitboard[turn][from_piece] = (bitboard[turn][from_piece] & ~to);
    if (to_piece != NONE) { // add back opponent piece (if any)
        bitboard[!turn][to_piece] = bitboard[!turn][to_piece] | to;
    }
    // add back your piece to 'from' square
    bitboard[turn][from_piece] = (bitboard[turn][from_piece] | from);
}

uint64_t Board::get_occupied(const Color color) const {
    const uint64_t* c = bitboard[color];
    return c[PAWN] | c[KNIGHT] | c[BISHOP] | c[ROOK] | c[QUEEN] | c[KING];
}

inline int get_knight_moves(const uint8_t i, uint8_t* indices) {
    int c = 0;
    if (rank(i) >= 3 && file(i) >= 'b') {
        indices[c++] = offset_idx(i, -1, -2);
    }
    if (rank(i) >= 3 && file(i) <= 'g') {
        indices[c++] = offset_idx(i, 1, -2);
    }
    if (rank(i) >= 2 && file(i) <= 'f') {
        indices[c++] = offset_idx(i, 2, -1);
    }
    if (rank(i) <= 7 && file(i) <= 'f') {
        indices[c++] = offset_idx(i, 2, 1);
    }
    if (rank(i) <= 6 && file(i) <= 'g') {
        indices[c++] = offset_idx(i, 1, 2);
    }
    if (rank(i) <= 6 && file(i) >= 'b') {
        indices[c++] = offset_idx(i, -1, 2);
    }
    if (rank(i) <= 7 && file(i) >= 'c') {
        indices[c++] = offset_idx(i, -2, 1);
    }
    if (rank(i) >= 2 && file(i) >= 'c') {
        indices[c++] = offset_idx(i, -2, -1);
    }
    return c;
}

inline int get_king_moves(const uint8_t i, uint8_t* indices) {
    int c = 0;
    const bool not_max_rank = rank(i) < 8, not_min_rank = rank(i) > 1, not_max_file = file(i) < 'h', not_min_file = file(i) > 'a';
    if (not_max_rank && not_max_file) {
        indices[c++] = offset_idx(i, 1, 1);
    }
    if (not_max_rank && not_min_file) {
        indices[c++] = offset_idx(i, -1, 1);
    }
    if (not_min_rank && not_max_file) {
        indices[c++] = offset_idx(i, 1, -1);
    }
    if (not_min_rank && not_min_file) {
        indices[c++] = offset_idx(i, -1, -1);
    }
    if (not_min_rank) {
        indices[c++] = offset_idx(i, 0, -1);
    }
    if (not_min_file) {
        indices[c++] = offset_idx(i, -1, 0);
    }
    if (not_max_rank) {
        indices[c++] = offset_idx(i, 0, 1);
    }
    if (not_max_file) {
        indices[c++] = offset_idx(i, 1, 0);
    }
    return c;
}

inline bool continuous_move_cond(const uint8_t i, const int f_off, const int r_off, const int f_incr, const int r_incr) {
    bool f_cond = false, r_cond = false;
    switch (f_incr) {
    case 1: f_cond = ((file(i) + f_off) <= 'h'); break;
    case 0: f_cond = true; break;
    case -1: f_cond = ((file(i) + f_off) >= 'a'); break;
    default: std::cout << "Invalid file increase!\n";
    }
    switch (r_incr) {
    case 1: r_cond = ((rank(i) + r_off) <= 8); break;
    case 0: r_cond = true; break;
    case -1: r_cond = ((rank(i) + r_off) >= 1); break;
    default: std::cout << "Invalid rank increase!\n";
    }
    return f_cond && r_cond;
}

inline void add_continuous_moves(const uint8_t i, const uint64_t any_occupied, const int f_incr, const int r_incr, uint8_t* indices, int& cnt) {
    for (int f_off = f_incr, r_off = r_incr; continuous_move_cond(i, f_off, r_off, f_incr, r_incr); f_off += f_incr, r_off += r_incr) {
        const uint8_t new_i = offset_idx(i, f_off, r_off);
        indices[cnt++] = new_i;
        if ((1ULL << new_i) & any_occupied)
            break;
    }
}

inline int get_diagonal_moves(const int i, const uint64_t any_occupied, uint8_t* indices) {
    int c = 0;
    add_continuous_moves(i, any_occupied, 1, 1, indices, c);
    add_continuous_moves(i, any_occupied, -1, 1, indices, c);
    add_continuous_moves(i, any_occupied, 1, -1, indices, c);
    add_continuous_moves(i, any_occupied, -1, -1, indices, c);
    return c;
}

inline int get_straight_moves(const int i, const uint64_t any_occupied, uint8_t* indices) {
    int c = 0;
    add_continuous_moves(i, any_occupied, 1, 0, indices, c);
    add_continuous_moves(i, any_occupied, 0, 1, indices, c);
    add_continuous_moves(i, any_occupied, -1, 0, indices, c);
    add_continuous_moves(i, any_occupied, 0, -1, indices, c);
    return c;
}

bool Board::is_in_check(const Color color) const {
    // pawns
    const uint8_t i = std::countr_zero(bitboard[color][KING]);
    if (file(i) > 'a' && (color ? (rank(i) > 1) : (rank(i) < 8)) && ((1ULL << offset_idx(i, -1, color ? -1 : 1)) & bitboard[!color][PAWN])) {
        return true;
    }
    if (file(i) < 'h' && (color ? (rank(i) > 1) : (rank(i) < 8)) && ((1ULL << offset_idx(i, 1, color ? -1 : 1)) & bitboard[!color][PAWN])) {
        return true;
    }
    // knights
    uint64_t n_sum = 0;
    uint8_t n_pos[8];
    const int n_cnt = get_knight_moves(i, n_pos);
    for (int j = 0; j < n_cnt; j++)
        n_sum |= 1ULL << n_pos[j];
    if (n_sum & bitboard[!color][KNIGHT])
        return true;

    const uint64_t any_occupied = get_occupied(!color) | get_occupied(color);
    // diagonal pieces
    uint64_t d_sum = 0;
    uint8_t d_pos[13];
    const int d_cnt = get_diagonal_moves(i, any_occupied, d_pos);
    for (int j = 0; j < d_cnt; j++)
        d_sum |= 1ULL << d_pos[j];
    if (d_sum & (bitboard[!color][BISHOP] | bitboard[!color][QUEEN]))
        return true;
    // straight pieces
    uint64_t s_sum = 0;
    uint8_t s_pos[13];
    const int s_cnt = get_straight_moves(i, any_occupied, s_pos);
    for (int j = 0; j < s_cnt; j++)
        s_sum |= 1ULL << s_pos[j];
    if (s_sum & (bitboard[!color][ROOK] | bitboard[!color][QUEEN]))
        return true;
    return false;
}

int Board::generate_moves(Move* moves) const {
    int c = 0; // move count

    const uint64_t occupied[2] = {get_occupied(WHITE), get_occupied(BLACK)};
    const uint64_t any_occupied = occupied[WHITE] | occupied[BLACK];

    for (uint8_t i = 0; i < 64; i++) {
        if (const uint64_t pos = 1ULL << i; !(any_occupied & pos)) {
            continue;
        } else if (bitboard[turn][PAWN] & pos) { // pawns
            const int col_sgn = (turn ? -1 : 1), col_start_rank = (turn ? 7 : 2);
            if (rank(i) == (turn ? 2 : 7) && !((1ULL << offset_idx(i, 0, col_sgn)) & any_occupied)) {
                // TODO: promote
                continue;
            }
            if (const uint8_t new_i = offset_idx(i, 0, col_sgn); !((1ULL << new_i) & any_occupied)) {
                moves[c++] = Move(i, new_i, PAWN, NONE);
                if (const uint8_t new_i2 = offset_idx(i, 0, 2*col_sgn); !((1ULL << new_i2) & any_occupied) && rank(i) == col_start_rank) {
                    moves[c++] = Move(i, new_i2, PAWN, NONE);
                }
            }
            if (file(i) > 'a') { // if it has a piece ahead-left
                if (const uint8_t new_i = offset_idx(i, -1, col_sgn); (1ULL << new_i) & occupied[!turn]) {
                    moves[c++] = Move(i, new_i, PAWN, get_piece(new_i, !turn));
                }
            }
            if (file(i) < 'h') { // if it has a piece ahead-right
                if (const uint8_t new_i = offset_idx(i, 1, col_sgn); (1ULL << new_i) & occupied[!turn]) {
                    moves[c++] = Move(i, new_i, PAWN, get_piece(new_i, !turn));
                }
            }
        } else if (bitboard[turn][KNIGHT] & pos) { // knights
            uint8_t n_moves[8];
            const int cnt = get_knight_moves(i, n_moves);
            for (int j = 0; j < cnt; j++)
                if (!(occupied[turn] & (1ULL << n_moves[j])))
                    moves[c++] = Move(i, n_moves[j], KNIGHT, get_piece(n_moves[j], !turn));
        } else if (bitboard[turn][KING] & pos) { // kings
            uint8_t k_moves[8];
            const int cnt = get_king_moves(i, k_moves);
            for (int j = 0; j < cnt; j++)
                if (!(occupied[turn] & (1ULL << k_moves[j])))
                    moves[c++] = Move(i, k_moves[j], KING, get_piece(k_moves[j], !turn));
        } else {
            if (const Piece diagonal_piece = ((bitboard[turn][BISHOP] & pos) ? BISHOP : QUEEN); bitboard[turn][diagonal_piece] & pos) { // bishops or queens
                uint8_t d_moves[13];
                const int cnt = get_diagonal_moves(i, any_occupied, d_moves);
                for (int j = 0; j < cnt; j++)
                    if (!(occupied[turn] & (1ULL << d_moves[j])))
                        moves[c++] = Move(i, d_moves[j], diagonal_piece, get_piece(d_moves[j], !turn));
            }
            if (const Piece straight_piece = (bitboard[turn][ROOK] & pos) ? ROOK : QUEEN; bitboard[turn][straight_piece] & pos) { // rooks or queens
                uint8_t s_moves[13];
                const int cnt = get_straight_moves(i, any_occupied, s_moves);
                for (int j = 0; j < cnt; j++)
                    if (!(occupied[turn] & (1ULL << s_moves[j])))
                        moves[c++] = Move(i, s_moves[j], straight_piece, get_piece(s_moves[j], !turn));
            }
        }
    }
    return c;
}

int Board::generate_legal_moves(Move* legal_moves) {
    int c = 0;
    Move unfiltered_moves[230];
    const int cnt = generate_moves(unfiltered_moves);
    for (int i = 0; i < cnt; i++) {
        make_move(unfiltered_moves[i]);
        if (!is_in_check(!turn)) {
            legal_moves[c++] = unfiltered_moves[i];
        }
        undo_move(unfiltered_moves[i]);
    }
    return c;
}

GameStatus Board::game_over() {
    Move legal_moves[230];
    if (generate_legal_moves(legal_moves)) {
        return Ongoing;
    }
    if (!is_in_check(turn)) {
        return Draw;
    }
    return turn ? WhiteWin : BlackWin;
}

// user input / initialization

void Board::setup_normal() {
    for (int i = 0; i < 8; i++) {
        if (i == 0 || i == 7) {
            add_piece(1ULL << (8 * i    ), i == 7, ROOK);
            add_piece(1ULL << (8 * i + 1), i == 7, KNIGHT);
            add_piece(1ULL << (8 * i + 2), i == 7, BISHOP);
            add_piece(1ULL << (8 * i + 3), i == 7, QUEEN);
            add_piece(1ULL << (8 * i + 4), i == 7, KING);
            add_piece(1ULL << (8 * i + 5), i == 7, BISHOP);
            add_piece(1ULL << (8 * i + 6), i == 7, KNIGHT);
            add_piece(1ULL << (8 * i + 7), i == 7, ROOK);
        } else if (i == 1 || i == 6) {
            for (int j = 0; j < 8; j++) {
                add_piece(1ULL << (8 * i + j), i == 6, PAWN);
            }
        }
    }
}

void Board::add_piece(const uint64_t pos, const Color color, const int piece) {
    bitboard[color][piece] |= pos;
}

Board::Board() : bitboard{}, turn(WHITE) {
    setup_normal();
}

Color Board::get_turn() const {
    return turn;
}

void Board::print_board() const {
    for (int i = 0; i < 8; i++) {
        std::cout << i+1 << " ";
        for (int j = 0; j < 8; j++) {
            Color color = WHITE;
            Piece piece = get_piece(8 * i + j, color);
            if (piece == NONE) {
                color = BLACK;
                piece = get_piece(8 * i + j, color);
            }
            std::cout << (piece == NONE ? ' ' : static_cast<char>(piece_map[piece] + (color ? '\0' : ' '))) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "  ";
    for (int i = 0; i < 8; i++) {
        std::cout << static_cast<char>('A' + i) << " ";
    }
    std::cout << "\n";
}

void Board::print_moves() {
    Move moves[230];
    const int move_count = generate_legal_moves(moves);
    for (int i = 0; i < move_count; i++) {
        moves[i].print();
        if (i % 3 == 2) std::cout << "\n";
    }
    std::cout << "\n";
}