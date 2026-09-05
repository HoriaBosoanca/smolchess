#include "board.h"
#include <cstdint>
#include <iostream>
#include <optional>

Board::Board(const std::optional<std::string>& fen) : bitboard{}, special_data(CASTLING_DATA_MASK), is_any_en_passant(false), turn_color(WHITE) {
    if (fen) {
        std::cout << "FEN not yet implemented!\n";
    } else {
        setup_normal();
    }
}

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

void Board::add_piece(const uint64_t sq, const Color color, const int piece) {
    bitboard[color][piece] |= sq;
}

int Board::generate_legal_moves(Move* legal_moves) {
    int c = 0;
    Move unfiltered_moves[MAX_MOVES];
    const int cnt = generate_moves(unfiltered_moves);
    for (int i = 0; i < cnt; i++) {
        const Board prev = *this;
        make_move(unfiltered_moves[i]);
        if (!is_in_check(!turn_color)) {
            legal_moves[c++] = unfiltered_moves[i];
        }
        *this = prev;
    }
    return c;
}

static int get_knight_moves(const uint8_t from_sq, uint8_t* indices) {
    int c = 0;
    if (rank(from_sq) >= 3 && file_char(from_sq) >= 'b') {
        indices[c++] = offset_idx(from_sq, -1, -2);
    }
    if (rank(from_sq) >= 3 && file_char(from_sq) <= 'g') {
        indices[c++] = offset_idx(from_sq, 1, -2);
    }
    if (rank(from_sq) >= 2 && file_char(from_sq) <= 'f') {
        indices[c++] = offset_idx(from_sq, 2, -1);
    }
    if (rank(from_sq) <= 7 && file_char(from_sq) <= 'f') {
        indices[c++] = offset_idx(from_sq, 2, 1);
    }
    if (rank(from_sq) <= 6 && file_char(from_sq) <= 'g') {
        indices[c++] = offset_idx(from_sq, 1, 2);
    }
    if (rank(from_sq) <= 6 && file_char(from_sq) >= 'b') {
        indices[c++] = offset_idx(from_sq, -1, 2);
    }
    if (rank(from_sq) <= 7 && file_char(from_sq) >= 'c') {
        indices[c++] = offset_idx(from_sq, -2, 1);
    }
    if (rank(from_sq) >= 2 && file_char(from_sq) >= 'c') {
        indices[c++] = offset_idx(from_sq, -2, -1);
    }
    return c;
}

static int get_king_moves(const uint8_t from_sq, uint8_t* indices) {
    int c = 0;
    const bool not_max_rank = rank(from_sq) < 8, not_min_rank = rank(from_sq) > 1, not_max_file = file_char(from_sq) < 'h', not_min_file = file_char(from_sq) > 'a';
    if (not_max_rank && not_max_file) {
        indices[c++] = offset_idx(from_sq, 1, 1);
    }
    if (not_max_rank && not_min_file) {
        indices[c++] = offset_idx(from_sq, -1, 1);
    }
    if (not_min_rank && not_max_file) {
        indices[c++] = offset_idx(from_sq, 1, -1);
    }
    if (not_min_rank && not_min_file) {
        indices[c++] = offset_idx(from_sq, -1, -1);
    }
    if (not_min_rank) {
        indices[c++] = offset_idx(from_sq, 0, -1);
    }
    if (not_min_file) {
        indices[c++] = offset_idx(from_sq, -1, 0);
    }
    if (not_max_rank) {
        indices[c++] = offset_idx(from_sq, 0, 1);
    }
    if (not_max_file) {
        indices[c++] = offset_idx(from_sq, 1, 0);
    }
    return c;
}

static bool continuous_move_cond(const uint8_t from_sq, const int f_off, const int r_off, const int f_incr, const int r_incr) {
    bool f_cond = false, r_cond = false;
    switch (f_incr) {
    case 1: f_cond = ((file_char(from_sq) + f_off) <= 'h'); break;
    case 0: f_cond = true; break;
    case -1: f_cond = ((file_char(from_sq) + f_off) >= 'a'); break;
    default: std::cout << "Invalid file increase!\n";
    }
    switch (r_incr) {
    case 1: r_cond = ((rank(from_sq) + r_off) <= 8); break;
    case 0: r_cond = true; break;
    case -1: r_cond = ((rank(from_sq) + r_off) >= 1); break;
    default: std::cout << "Invalid rank increase!\n";
    }
    return f_cond && r_cond;
}

static void add_continuous_moves(const uint8_t from_sq, const uint64_t any_occupied, const int f_incr, const int r_incr, uint8_t* indices, int& cnt) {
    for (int f_off = f_incr, r_off = r_incr; continuous_move_cond(from_sq, f_off, r_off, f_incr, r_incr); f_off += f_incr, r_off += r_incr) {
        const uint8_t new_i = offset_idx(from_sq, f_off, r_off);
        indices[cnt++] = new_i;
        if ((1ULL << new_i) & any_occupied)
            break;
    }
}

static int get_diagonal_moves(const uint8_t from_sq, const uint64_t any_occupied, uint8_t* indices) {
    int c = 0;
    add_continuous_moves(from_sq, any_occupied, 1, 1, indices, c);
    add_continuous_moves(from_sq, any_occupied, -1, 1, indices, c);
    add_continuous_moves(from_sq, any_occupied, 1, -1, indices, c);
    add_continuous_moves(from_sq, any_occupied, -1, -1, indices, c);
    return c;
}

static int get_straight_moves(const uint8_t from_sq, const uint64_t any_occupied, uint8_t* indices) {
    int c = 0;
    add_continuous_moves(from_sq, any_occupied, 1, 0, indices, c);
    add_continuous_moves(from_sq, any_occupied, 0, 1, indices, c);
    add_continuous_moves(from_sq, any_occupied, -1, 0, indices, c);
    add_continuous_moves(from_sq, any_occupied, 0, -1, indices, c);
    return c;
}

int Board::generate_moves(Move* moves) const {
    int c = 0; // move count

    const uint64_t occupied[2] = {get_occupied(WHITE), get_occupied(BLACK)};
    const uint64_t any_occupied = occupied[WHITE] | occupied[BLACK];

    for (uint8_t from_sq = 0; from_sq < 64; from_sq++) {
        if (const uint64_t from_bb = 1ULL << from_sq; !(any_occupied & from_bb)) {
        } else if (bitboard[turn_color][PAWN] & from_bb) { // pawns
            const int col_sgn = (turn_color ? -1 : 1), col_start_rank = (turn_color ? 7 : 2);
            bool promotion = false;
            if (rank(from_sq) == (turn_color ? 2 : 7))
                promotion = true;
            if (auto en_passant = get_nearby_en_passant(from_sq))
                moves[c++] = Move(from_sq, en_passant.value(), PAWN, get_piece(en_passant.value(), !turn_color), EN_PASSANT);
            if (const uint8_t new_i = offset_idx(from_sq, 0, col_sgn); !((1ULL << new_i) & any_occupied)) {
                if (promotion) {
                    for (int move_type = KNIGHT_PROMOTION; move_type <= QUEEN_PROMOTION; move_type++)
                        moves[c++] = Move(from_sq, new_i, PAWN, get_piece(new_i, !turn_color), static_cast<MoveType>(move_type));
                } else {
                    moves[c++] = Move(from_sq, new_i, PAWN);
                }
                if (const uint8_t new_i2 = offset_idx(from_sq, 0, 2*col_sgn); rank(from_sq) == col_start_rank && !((1ULL << new_i2) & any_occupied)) {
                    moves[c++] = Move(from_sq, new_i2, PAWN);
                }
            }
            if (file_char(from_sq) > 'a') { // if it has a piece ahead-left
                if (const uint8_t new_i = offset_idx(from_sq, -1, col_sgn); (1ULL << new_i) & occupied[!turn_color]) {
                    if (promotion) {
                        // TODO: can be refactored into an add_all_promotions() function
                        for (int move_type = KNIGHT_PROMOTION; move_type <= QUEEN_PROMOTION; move_type++)
                            moves[c++] = Move(from_sq, new_i, PAWN, get_piece(new_i, !turn_color), static_cast<MoveType>(move_type));
                    } else {
                        moves[c++] = Move(from_sq, new_i, PAWN, get_piece(new_i, !turn_color));
                    }
                }
            }
            if (file_char(from_sq) < 'h') { // if it has a piece ahead-right
                if (const uint8_t new_i = offset_idx(from_sq, 1, col_sgn); (1ULL << new_i) & occupied[!turn_color]) {
                    if (promotion) {
                        for (int move_type = KNIGHT_PROMOTION; move_type <= QUEEN_PROMOTION; move_type++)
                            moves[c++] = Move(from_sq, new_i, PAWN, get_piece(new_i, !turn_color), static_cast<MoveType>(move_type));
                    } else {
                        moves[c++] = Move(from_sq, new_i, PAWN, get_piece(new_i, !turn_color));
                    }
                }
            }
        } else if (bitboard[turn_color][KNIGHT] & from_bb) { // knights
            uint8_t n_moves[8];
            const int cnt = get_knight_moves(from_sq, n_moves);
            for (int j = 0; j < cnt; j++)
                if (!(occupied[turn_color] & (1ULL << n_moves[j])))
                    moves[c++] = Move(from_sq, n_moves[j], KNIGHT, get_piece(n_moves[j], !turn_color));
        } else if (bitboard[turn_color][KING] & from_bb) { // kings
            uint8_t k_moves[8];
            const int cnt = get_king_moves(from_sq, k_moves);
            for (int j = 0; j < cnt; j++)
                if (!(occupied[turn_color] & (1ULL << k_moves[j])))
                    moves[c++] = Move(from_sq, k_moves[j], KING, get_piece(k_moves[j], !turn_color));
            if (auto to = get_castle_move(true))
                moves[c++] = Move(from_sq, to.value(), KING, NONE, QUEEN_CASTLING);
            if (auto to = get_castle_move(false))
                moves[c++] = Move(from_sq, to.value(), KING, NONE, KING_CASTLING);
        } else {
            if (const Piece diagonal_piece = ((bitboard[turn_color][BISHOP] & from_bb) ? BISHOP : QUEEN); bitboard[turn_color][diagonal_piece] & from_bb) { // bishops or queens
                uint8_t d_moves[14];
                const int cnt = get_diagonal_moves(from_sq, any_occupied, d_moves);
                for (int j = 0; j < cnt; j++)
                    if (!(occupied[turn_color] & (1ULL << d_moves[j])))
                        moves[c++] = Move(from_sq, d_moves[j], diagonal_piece, get_piece(d_moves[j], !turn_color));
            }
            if (const Piece straight_piece = (bitboard[turn_color][ROOK] & from_bb) ? ROOK : QUEEN; bitboard[turn_color][straight_piece] & from_bb) { // rooks or queens
                uint8_t s_moves[14];
                const int cnt = get_straight_moves(from_sq, any_occupied, s_moves);
                for (int j = 0; j < cnt; j++)
                    if (!(occupied[turn_color] & (1ULL << s_moves[j])))
                        moves[c++] = Move(from_sq, s_moves[j], straight_piece, get_piece(s_moves[j], !turn_color));
            }
        }
    }
    return c;
}

uint64_t Board::get_occupied(const Color color) const {
    const uint64_t* c = bitboard[color];
    return c[PAWN] | c[KNIGHT] | c[BISHOP] | c[ROOK] | c[QUEEN] | c[KING];
}

Piece Board::get_piece(const uint8_t pos, const Color color) const {
    const uint64_t pos64 = (1ULL << pos);
    for (int p = PAWN; p <= KING; p++)
        if (bitboard[color][p] & pos64)
            return static_cast<Piece>(p);
    return NONE;
}

std::optional<uint8_t> Board::get_nearby_en_passant(const uint8_t from_sq) const {
    if (!is_any_en_passant)
        return std::nullopt;
    const uint8_t en_passant = special_data & EN_PASSANT_DATA_MASK;
    if (const Color en_passant_color = en_passant >= 8; en_passant_color == turn_color)
        return std::nullopt;
    // the color for who the en passant is saved is the piece to be captured =/= the color of the piece to be moved
    // if black, get white en passant file; if white, get black en passant file
    const uint8_t en_passant_file = static_cast<uint8_t>(en_passant - (turn_color ? 0 : 8));
    // to en passant, you also need to be on the correct rank. As a black pawn, that is rank 4; for white it's 5
    const int8_t needed_rank = turn_color ? 4 : 5;
    if (file_int(from_sq) + 1 == en_passant_file && rank(from_sq) == needed_rank) {
        return offset_idx(from_sq, 1, turn_color ? -1 : 1);
    }
    if (file_int(from_sq) - 1 == en_passant_file && rank(from_sq) == needed_rank) {
        return offset_idx(from_sq, -1, turn_color ? -1 : 1);
    }
    return std::nullopt;
}

// TODO: is_attacked() conditions should be tested more
std::optional<uint8_t> Board::get_castle_move(const bool queen_side) const {
    if (!(special_data & CASTLING_DATA_MASK & ((1 << (queen_side ? 4 : 5)) << (turn_color ? 2 : 0)))) { // verify castling rights
        return std::nullopt;
    }
    const uint64_t occupied = get_occupied(turn_color) | get_occupied(!turn_color);
    if (queen_side &&
        !(occupied & (1ULL<<(turn_color?57:1) | 1ULL<<(turn_color?58:2) | 1ULL<<(turn_color?59:3))) &&
        !(is_attacked(turn_color?58:2, turn_color) || is_attacked(turn_color?59:3, turn_color || is_in_check(turn_color)))) return turn_color?58:2;
    if (!queen_side &&
        !(occupied & (1ULL<<(turn_color?61:5) | 1ULL<<(turn_color?62:6))) &&
        !(is_attacked(turn_color?61:5, turn_color) || is_attacked(turn_color?62:6, turn_color || is_in_check(turn_color)))) return turn_color?62:6;

    return std::nullopt;
}

bool Board::is_in_check(const Color color) const {
    return is_attacked(static_cast<uint8_t>(std::countr_zero(bitboard[color][KING])), color);
}

// color = attacked piece color
bool Board::is_attacked(const uint8_t attacked_sq, const Color color) const {
    // pawns
    if (file_char(attacked_sq) > 'a' && (color ? (rank(attacked_sq) > 1) : (rank(attacked_sq) < 8)) && ((1ULL << offset_idx(attacked_sq, -1, color ? -1 : 1)) & bitboard[!color][PAWN])) {
        return true;
    }
    if (file_char(attacked_sq) < 'h' && (color ? (rank(attacked_sq) > 1) : (rank(attacked_sq) < 8)) && ((1ULL << offset_idx(attacked_sq, 1, color ? -1 : 1)) & bitboard[!color][PAWN])) {
        return true;
    }
    // knights
    uint64_t n_sum = 0;
    uint8_t n_pos[8];
    const int n_cnt = get_knight_moves(attacked_sq, n_pos);
    for (int j = 0; j < n_cnt; j++)
        n_sum |= 1ULL << n_pos[j];
    if (n_sum & bitboard[!color][KNIGHT])
        return true;

    const uint64_t any_occupied = get_occupied(!color) | get_occupied(color);
    // diagonal pieces
    uint64_t d_sum = 0;
    uint8_t d_pos[14];
    const int d_cnt = get_diagonal_moves(attacked_sq, any_occupied, d_pos);
    for (int j = 0; j < d_cnt; j++)
        d_sum |= 1ULL << d_pos[j];
    if (d_sum & (bitboard[!color][BISHOP] | bitboard[!color][QUEEN]))
        return true;
    // straight pieces
    uint64_t s_sum = 0;
    uint8_t s_pos[14];
    const int s_cnt = get_straight_moves(attacked_sq, any_occupied, s_pos);
    for (int j = 0; j < s_cnt; j++)
        s_sum |= 1ULL << s_pos[j];
    if (s_sum & (bitboard[!color][ROOK] | bitboard[!color][QUEEN]))
        return true;
    // kings
    uint64_t k_sum = 0;
    uint8_t k_moves[8];
    const int k_cnt = get_king_moves(attacked_sq, k_moves);
    for (int j = 0; j < k_cnt; j++)
        k_sum |= 1ULL << k_moves[j];
    if (k_sum & bitboard[!color][KING])
        return true;

    return false;
}

void Board::make_move(const Move move) {
    const uint8_t from_sq = move.from(), to_sq = move.to(), from_piece = move.from_piece(), to_piece = move.to_piece();
    const MoveType move_type = move.move_type();

    bitboard[turn_color][from_piece] = (bitboard[turn_color][from_piece] & ~(1ULL << from_sq)) | (1ULL << to_sq);
    if (to_piece != NONE) {
        bitboard[!turn_color][to_piece] &= ~(1ULL << to_sq);
    }
    // save en passant
    clear_all_en_passant_availability();
    if (from_piece == PAWN && rank(to_sq)-rank(from_sq) == (turn_color ? -2 : 2)) {
        add_en_passant_availability(to_sq);
    }
    // remove castling rights
    if (from_piece == KING) {
        special_data &= (turn_color ? 0b00111111 : 0b11001111);
    }
    if (from_piece == ROOK) {
        special_data &= (turn_color ? (from_sq == 56 ? 0b10111111 : 0b01111111) : (from_sq == 0 ? 0b11101111 : 0b11011111));
    }
    if (to_piece == ROOK) {
        if (to_sq == (turn_color ? 0 : 56)) {
            special_data &= (turn_color ? 0b11101111 : 0b10111111);
        }
        if (to_sq == (turn_color ? 7 : 63)) {
            special_data &= (turn_color ? 0b11011111 : 0b01111111);
        }
    }
    switch (move_type) {
        case REGULAR: {
            break;
        }
        case EN_PASSANT: {
            bitboard[!turn_color][PAWN] &= ~(1ULL << offset_idx(to_sq, 0, turn_color ? 1 : -1));
            break;
        }
        case QUEEN_CASTLING: {
            bitboard[turn_color][ROOK] = (bitboard[turn_color][ROOK] & ~(1ULL << (turn_color ? 56 : 0))) | 1ULL << (turn_color ? 59 : 3);
            break;
        }
        case KING_CASTLING: {
            bitboard[turn_color][ROOK] = (bitboard[turn_color][ROOK] & ~(1ULL << (turn_color ? 63 : 7))) | 1ULL << (turn_color ? 61 : 5);
            break;
        }
        case KNIGHT_PROMOTION: {
            bitboard[turn_color][PAWN] &= ~(1ULL << to_sq);
            bitboard[turn_color][KNIGHT] |= 1ULL << to_sq;
            break;
        }
        case BISHOP_PROMOTION: {
            bitboard[turn_color][PAWN] &= ~(1ULL << to_sq);
            bitboard[turn_color][BISHOP] |= 1ULL << to_sq;
            break;
        }
        case ROOK_PROMOTION: {
            bitboard[turn_color][PAWN] &= ~(1ULL << to_sq);
            bitboard[turn_color][ROOK] |= 1ULL << to_sq;
            break;
        }
        case QUEEN_PROMOTION: {
            bitboard[turn_color][PAWN] &= ~(1ULL << to_sq);
            bitboard[turn_color][QUEEN] |= 1ULL << to_sq;
            break;
        }
        default: {
            std::cout << "Unrecognized move type?\n";
            exit(-1);
        }
    }
    turn_color = !turn_color;
}

void Board::clear_all_en_passant_availability() {
    is_any_en_passant = false;
}

// 0..7 -> white en passant; 8..15 -> black en passant
// the color for who the en passant is saved is the piece to be captured
void Board::add_en_passant_availability(const uint8_t capturable_sq) {
    is_any_en_passant = true;
    const uint8_t en_passant_value = static_cast<uint8_t>(file_int(capturable_sq) + (turn_color ? 8 : 0));
    if (en_passant_value > EN_PASSANT_DATA_MASK)
        throw std::runtime_error("en passant value out of range");
    // set en passant value to 0 so that en passant data is added onto 0
    special_data &= CASTLING_DATA_MASK;
    // add en passant data
    special_data |= en_passant_value;
}

GameStatus Board::game_over() {
    if (Move legal_moves[MAX_MOVES]; generate_legal_moves(legal_moves) > 0) {
        return Ongoing;
    }
    if (!is_in_check(turn_color)) {
        return Draw;
    }
    return turn_color ? WhiteWin : BlackWin;
}