#include <cstdint>
#include <iostream>
#include "board.h"
#include "board_helpers.h"

void Board::setup_normal() {
    for (int i = 0; i < 8; i++) {
        if (i == 0 || i == 7) {
            add_piece(1ULL << (8 * i    ), i == 0 ? WHITE : BLACK, ROOK);
            add_piece(1ULL << (8 * i + 1), i == 0 ? WHITE : BLACK, KNIGHT);
            add_piece(1ULL << (8 * i + 2), i == 0 ? WHITE : BLACK, BISHOP);
            add_piece(1ULL << (8 * i + 3), i == 0 ? WHITE : BLACK, QUEEN);
            add_piece(1ULL << (8 * i + 4), i == 0 ? WHITE : BLACK, KING);
            add_piece(1ULL << (8 * i + 5), i == 0 ? WHITE : BLACK, BISHOP);
            add_piece(1ULL << (8 * i + 6), i == 0 ? WHITE : BLACK, KNIGHT);
            add_piece(1ULL << (8 * i + 7), i == 0 ? WHITE : BLACK, ROOK);
        } else if (i == 1 || i == 6) {
            for (int j = 0; j < 8; j++) {
                add_piece(1ULL << (8 * i + j), i == 1 ? WHITE : BLACK, PAWN);
            }
        }
    }
}

void Board::add_piece(const uint64_t pos, const Color color, const int piece) {
    bitboard[color][piece] |= pos;
}

bool Board::check_move_legality(const Move move, const Color color) const {
    for (int i = 0; i < move_count[color]; i++)
        if (move.move == moves[color][i].move)
            return true;
    std::cout << "Illegal move!\n";
    return false;
}

bool Board::move_unknown_str(std::string& move, const Color color) {
    if (!parse_move_format(move)) {
        return false;
    }
    const uint8_t from = (move[1]-'1')*8+move[0]-'a', to = (move[3]-'1')*8+move[2]-'a';
    auto [from_piece, from_color] = get_piece(1ULL << from);
    if (from_piece == NONE || from_color != color) {
        std::cout << "Not your piece!\n";
        return false;
    }
    if (!check_move_legality(Move(from, to, from_piece), color)) {
        return false;
    }
    move_unknown(1ULL << from, 1ULL << to);
    return true;
}

Piece_and_Color Board::get_piece(const uint64_t pos) const {
    for (int c = WHITE; c <= BLACK; c++)
        for (int p = PAWN; p <= KING; p++)
            if (bitboard[c][p] & pos)
                return Piece_and_Color{(Piece) p, (Color) c};
    return Piece_and_Color{NONE, WHITE};
}

void Board::print_moves(const Color color) const {
    for (int i = 0; i < move_count[color]; i++) {
        moves[color][i].print();
    }
}

uint64_t Board::get_occupied(const Color color) const {
    const uint64_t* c = bitboard[color];
    return c[PAWN] | c[KNIGHT] | c[BISHOP] | c[ROOK] | c[QUEEN] | c[KING];
}

void Board::add_move(const Move move, const Color color) {
    moves[color][move_count[color]++] = move;
}

bool continuous_move_condition(const uint8_t i, const int file_offset, const int rank_offset, const int file_increase, const int rank_increase) {
    bool file_condition = false, rank_condition = false;
    switch (file_increase) {
        case 1: file_condition = ((file(i) + file_offset) <= 'h'); break;
        case 0: file_condition = true; break;
        case -1: file_condition = ((file(i) + file_offset) >= 'a'); break;
        default: std::cout << "Invalid file increase!\n";
    }
    switch (rank_increase) {
    case 1: rank_condition = ((rank(i) + rank_offset) <= 8); break;
    case 0: rank_condition = true; break;
    case -1: rank_condition = ((rank(i) + rank_offset) >= 1); break;
    default: std::cout << "Invalid rank increase!\n";
    }
    return file_condition && rank_condition;
}

void Board::add_continuous_move(const uint8_t i, const uint64_t pos, const Color color, const Piece piece, const uint64_t* occupied,
    const int file_increase, const int rank_increase) {
    for (int file_offset = file_increase, rank_offset = rank_increase; continuous_move_condition(i, file_offset, rank_offset, file_increase, rank_increase);
        file_offset += file_increase, rank_offset += rank_increase) {
        if (offset_pos(pos, file_offset, rank_offset) & (occupied[WHITE] | occupied[BLACK])) {
            if (offset_pos(pos, file_offset, rank_offset) & occupied[!color]) {
                add_move(Move(i, offset_idx(i, file_offset, rank_offset), piece), color);
            }
            break;
        }
        add_move(Move(i, offset_idx(i, file_offset, rank_offset), piece), color);
    }
}

void Board::generate_moves(const Color color) {
    move_count[color] = 0; // reset moves

    const uint64_t occupied[2] = {get_occupied(WHITE), get_occupied(BLACK)};
    const uint64_t any_occupied = occupied[WHITE] | occupied[BLACK];

    for (uint8_t i = 0; i < 64; i++) {
        if (const uint64_t pos = 1ULL << i; bitboard[color][PAWN] & pos) { // pawns
            if (color == WHITE) {
                if (rank(i) == 8) { // if it's rank 8
                    // TODO: promote
                    continue;
                }
                if (!(offset_pos(pos, 0, 1) & (occupied[WHITE] | occupied[BLACK]))) {
                    add_move(Move(i, offset_idx(i, 0, 1), PAWN), color);
                }
                if ((file(i) > 'a') && (offset_pos(pos, -1, 1) & occupied[BLACK])) { // if it has a piece ahead-left
                    add_move(Move(i, offset_idx(i, -1, 1), PAWN), color);
                }
                if ((file(i) < 'h') && (offset_pos(pos, 1, 1) & occupied[BLACK])) { // if it has a piece ahead-right
                    add_move(Move(i, offset_idx(i, 1, 1), PAWN), color);
                }
            } else {
                if (rank(i) == 1) { // if it's rank 1
                    // TODO: promote
                    continue;
                }
                if (!(offset_pos(pos, 0, -1) & any_occupied)) { // if it has space ahead
                    add_move(Move(i, offset_idx(i, 0, -1), PAWN), color);
                }
                if ((file(i) > 'a') && (offset_pos(pos, -1, -1) & occupied[WHITE])) { // if it has a piece ahead-left
                    add_move(Move(i, offset_idx(i, -1, -1), PAWN), color);
                }
                if ((file(i) < 'h') && (offset_pos(pos, 1, -1) & occupied[WHITE])) { // if it has a piece ahead-right
                    add_move(Move(i, offset_idx(i, 1, -1), PAWN), color);
                }
            }
        } else if (bitboard[color][KNIGHT] & pos) { // knights
            if (rank(i) >= 3 && file(i) >= 'b' && !(occupied[color] & offset_pos(pos, -1, -2))) {
                add_move(Move(i, offset_idx(i, -1, -2), KNIGHT), color);
            }
            if (rank(i) >= 3 && file(i) <= 'g' && !(occupied[color] & offset_pos(pos, 1, -2))) {
                add_move(Move(i, offset_idx(i, 1, -2), KNIGHT), color);
            }
            if (rank(i) >= 2 && file(i) <= 'f' && !(occupied[color] & offset_pos(pos, 2, -1))) {
                add_move(Move(i, offset_idx(i, 2, -1), KNIGHT), color);
            }
            if (rank(i) <= 7 && file(i) <= 'f' && !(occupied[color] & offset_pos(pos, 2, 1))) {
                add_move(Move(i, offset_idx(i, 2, 1), KNIGHT), color);
            }
            if (rank(i) <= 6 && file(i) <= 'g' && !(occupied[color] & offset_pos(pos, 1, 2))) {
                add_move(Move(i, offset_idx(i, 1, 2), KNIGHT), color);
            }
            if (rank(i) <= 6 && file(i) >= 'b' && !(occupied[color] & offset_pos(pos, -1, 2))) {
                add_move(Move(i, offset_idx(i, -1, 2), KNIGHT), color);
            }
            if (rank(i) <= 7 && file(i) >= 'c' && !(occupied[color] & offset_pos(pos, -2, 1))) {
                add_move(Move(i, offset_idx(i, -2, 1), KNIGHT), color);
            }
            if (rank(i) >= 2 && file(i) >= 'c' && !(occupied[color] & offset_pos(pos, -2, -1))) {
                add_move(Move(i, offset_idx(i, -2, -1), KNIGHT), color);
            }
        } else if (bitboard[color][KING] & pos) { // kings
            const bool not_max_rank = rank(i) < 8, not_min_rank = rank(i) > 1, not_max_file = file(i) < 'h', not_min_file = file(i) > 'a';
            if (not_max_rank && not_max_file && !(occupied[color] & offset_pos(pos, 1, 1))) {
                add_move(Move(i, offset_idx(i, 1, 1), KING), color);
            }
            if (not_max_rank && not_min_file && !(occupied[color] & offset_pos(pos, -1, 1))) {
                add_move(Move(i, offset_idx(i, -1, 1), KING), color);
            }
            if (not_min_rank && not_max_file && !(occupied[color] & offset_pos(pos, 1, -1))) {
                add_move(Move(i, offset_idx(i, 1, -1), KING), color);
            }
            if (not_min_rank && not_min_file && !(occupied[color] & offset_pos(pos, -1, -1))) {
                add_move(Move(i, offset_idx(i, -1, -1), KING), color);
            }
            if (not_min_rank && !(occupied[color] & offset_pos(pos, 0, -1))) {
                add_move(Move(i, offset_idx(i, 0, -1), KING), color);
            }
            if (not_min_file && !(occupied[color] & offset_pos(pos, -1, 0))) {
                add_move(Move(i, offset_idx(i, -1, 0), KING), color);
            }
            if (not_max_rank && !(occupied[color] & offset_pos(pos, 0, 1))) {
                add_move(Move(i, offset_idx(i, 0, 1), KING), color);
            }
            if (not_max_file && !(occupied[color] & offset_pos(pos, 1, 0))) {
                add_move(Move(i, offset_idx(i, 1, 0), KING), color);
            }
        } else {
            if (const Piece diagonal_piece = ((bitboard[color][BISHOP] & pos) ? BISHOP : QUEEN); bitboard[color][diagonal_piece] & pos) { // bishops or queens
                add_continuous_move(i, pos, color, diagonal_piece, occupied, 1, 1);
                add_continuous_move(i, pos, color, diagonal_piece, occupied, -1, 1);
                add_continuous_move(i, pos, color, diagonal_piece, occupied, -1, -1);
                add_continuous_move(i, pos, color, diagonal_piece, occupied, 1, -1);
            }
            if (const Piece straight_piece = (bitboard[color][ROOK] & pos) ? ROOK : QUEEN; bitboard[color][straight_piece] & pos) { // rooks or queens
                add_continuous_move(i, pos, color, straight_piece, occupied, 1, 0);
                add_continuous_move(i, pos, color, straight_piece, occupied, 0, 1);
                add_continuous_move(i, pos, color, straight_piece, occupied, -1, 0);
                add_continuous_move(i, pos, color, straight_piece, occupied, 0, -1);
            }
        }
    }
}

void Board::move_unknown(const uint64_t from, const uint64_t to) {
    for (int c = WHITE; c <= BLACK; c++) {
        for (int p = PAWN; p <= KING; p++) {
            if (bitboard[c][p] & from) {
                bitboard[c][p] = (bitboard[c][p] & ~from) | to;
                return;
            }
        }
    }
}

Move::Move() {
    move = 0;
}

Move::Move(const uint8_t from, const uint8_t to, const Piece piece) {
    move = from | (to << 6) | (piece << 12);
}

void Move::print() const {
    constexpr int mask = 0b111111;
    const int from = move & mask;
    const int to = (move >> 6) & mask;
    const int piece = (move >> 12) & mask;
    std::cout << piece_map[piece] << " " << file(from) << rank(from) << " " << file(to) << rank(to) << " " << "\n";
}