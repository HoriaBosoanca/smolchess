#include <cstdint>
#include <iostream>
#include "board.h"

// optimized

Move::Move() : move(0), pieces(0) {}

Move::Move(const uint8_t from, const uint8_t to, const Piece from_piece, const Piece to_piece) {
    move = from | (to << 6); // 16 - 6x2 = 4 bits left
    pieces = from_piece | (to_piece << 3); // 8 - 3x2 = 2 bits left
}

uint8_t Move::get_from() const {
    return move & 0b111111;
}

uint8_t Move::get_to() const {
    return (move >> 6) & 0b111111;
}

uint8_t Move::get_from_piece() const {
    return pieces & 0b111;
}

uint8_t Move::get_to_piece() const {
    return (pieces >> 3) & 0b111;
}

void Move::print() const {
    std::cout << piece_map[get_from_piece()] << " " << file(get_from()) << rank(get_from()) << file(get_to()) << rank(get_to()) << " " << piece_map[get_to_piece()] << "\n";
}

Piece Board::get_piece(const uint64_t pos, const Color color) const {
    for (int p = PAWN; p <= KING; p++)
        if (bitboard[color][p] & pos)
            return Piece(p);
    return NONE;
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

void Board::add_continuous_move(const uint8_t i, const uint64_t pos, const Piece piece, const uint64_t* occupied,
    const int file_increase, const int rank_increase, Move* moves, int& c) const {
    for (int file_offset = file_increase, rank_offset = rank_increase; continuous_move_condition(i, file_offset, rank_offset, file_increase, rank_increase);
        file_offset += file_increase, rank_offset += rank_increase) {
        if (const uint64_t new_pos = offset_pos(pos, file_offset, rank_offset);
            new_pos & (occupied[WHITE] | occupied[BLACK])) {
            if (new_pos & occupied[!turn]) {
                moves[c++] = Move(i, offset_idx(i, file_offset, rank_offset), piece, get_piece(new_pos, Color(!turn)));
            }
            break;
        }
        moves[c++] = Move(i, offset_idx(i, file_offset, rank_offset), piece, NONE);
    }
}

void Board::make_move(const Move move) {
    const uint64_t from = 1ULL << move.get_from(), to = 1ULL << move.get_to();
    const uint8_t from_piece = move.get_from_piece(), to_piece = move.get_to_piece();
    bitboard[turn][from_piece] = (bitboard[turn][from_piece] & ~from) | to;
    if (to_piece != NONE) {
        bitboard[Color(!turn)][to_piece] = bitboard[Color(!turn)][to_piece] & ~to;
    }
    turn = Color(!turn);
}

uint64_t Board::get_occupied(const Color color) const {
    const uint64_t* c = bitboard[color];
    return c[PAWN] | c[KNIGHT] | c[BISHOP] | c[ROOK] | c[QUEEN] | c[KING];
}

int Board::generate_moves(Move* moves) const {
    int c = 0; // move count

    const uint64_t occupied[2] = {get_occupied(WHITE), get_occupied(BLACK)};
    const uint64_t any_occupied = occupied[WHITE] | occupied[BLACK];

    for (uint8_t i = 0; i < 64; i++) {
        if (const uint64_t pos = 1ULL << i; bitboard[turn][PAWN] & pos) { // pawns
            if (turn == WHITE) {
                if (rank(i) == 8) { // if it's rank 8
                    // TODO: promote
                    continue;
                }
                if (!(offset_pos(pos, 0, 1) & any_occupied)) {
                    moves[c++] = Move(i, offset_idx(i, 0, 1), PAWN, NONE);
                    if (!(offset_pos(pos, 0, 2) & any_occupied) && rank(i) == 2) {
                        moves[c++] = Move(i, offset_idx(i, 0, 2), PAWN, NONE);
                    }
                }
                if (file(i) > 'a') { // if it has a piece ahead-left
                    if (const uint64_t new_pos = offset_pos(pos, -1, 1); new_pos & occupied[BLACK]) {
                        moves[c++] = Move(i, offset_idx(i, -1, 1), PAWN, get_piece(new_pos, BLACK));
                    }
                }
                if (file(i) < 'h') { // if it has a piece ahead-right
                    if (const uint64_t new_pos = offset_pos(pos, 1, 1); new_pos & occupied[BLACK]) {
                        moves[c++] = Move(i, offset_idx(i, 1, 1), PAWN, get_piece(new_pos, BLACK));
                    }
                }
            } else {
                if (rank(i) == 1) { // if it's rank 1
                    // TODO: promote
                    continue;
                }
                if (!(offset_pos(pos, 0, -1) & any_occupied)) { // if it has space ahead
                    moves[c++] = Move(i, offset_idx(i, 0, -1), PAWN, NONE);
                    if (!(offset_pos(pos, 0, -2) & any_occupied) && rank(i) == 7) {
                        moves[c++] = Move(i, offset_idx(i, 0, -2), PAWN, NONE);
                    }
                }
                if (file(i) > 'a') { // if it has a piece ahead-left
                    if (const uint64_t new_pos = offset_pos(pos, -1, -1); new_pos & occupied[WHITE]) {
                        moves[c++] = Move(i, offset_idx(i, -1, -1), PAWN, get_piece(new_pos, WHITE));
                    }
                }
                if (file(i) < 'h') { // if it has a piece ahead-right
                    if (const uint64_t new_pos = offset_pos(pos, 1, -1); new_pos & occupied[WHITE]) {
                        moves[c++] = Move(i, offset_idx(i, 1, -1), PAWN, get_piece(new_pos, WHITE));
                    }
                }
            }
        } else if (bitboard[turn][KNIGHT] & pos) { // knights
            if (const uint64_t new_pos = offset_pos(pos, -1, -2);
                rank(i) >= 3 && file(i) >= 'b' && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, -1, -2), KNIGHT, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, 1, -2);
                rank(i) >= 3 && file(i) <= 'g' && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 1, -2), KNIGHT, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, 2, -1);
                rank(i) >= 2 && file(i) <= 'f' && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 2, -1), KNIGHT, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, 2, 1);
                rank(i) <= 7 && file(i) <= 'f' && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 2, 1), KNIGHT, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, 1, 2);
                rank(i) <= 6 && file(i) <= 'g' && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 1, 2), KNIGHT, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, -1, 2);
                rank(i) <= 6 && file(i) >= 'b' && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, -1, 2), KNIGHT, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, -2, 1);
                rank(i) <= 7 && file(i) >= 'c' && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, -2, 1), KNIGHT, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, -2, -1);
                rank(i) >= 2 && file(i) >= 'c' && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, -2, -1), KNIGHT, get_piece(new_pos, Color(!turn)));
            }
        } else if (bitboard[turn][KING] & pos) { // kings
            const bool not_max_rank = rank(i) < 8, not_min_rank = rank(i) > 1, not_max_file = file(i) < 'h', not_min_file = file(i) > 'a';
            if (const uint64_t new_pos = offset_pos(pos, 1, 1);
                not_max_rank && not_max_file && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 1, 1), KING, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, -1, 1);
                not_max_rank && not_min_file && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, -1, 1), KING, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, 1, -1);
                not_min_rank && not_max_file && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 1, -1), KING, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, -1, -1);
                not_min_rank && not_min_file && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, -1, -1), KING, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, 0, -1);
                not_min_rank && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 0, -1), KING, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, -1, 0);
                not_min_file && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, -1, 0), KING, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, 0, 1);
                not_max_rank && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 0, 1), KING, get_piece(new_pos, Color(!turn)));
            }
            if (const uint64_t new_pos = offset_pos(pos, 1, 0);
                not_max_file && !(occupied[turn] & new_pos)) {
                moves[c++] = Move(i, offset_idx(i, 1, 0), KING, get_piece(new_pos, Color(!turn)));
            }
        } else {
            if (const Piece diagonal_piece = ((bitboard[turn][BISHOP] & pos) ? BISHOP : QUEEN); bitboard[turn][diagonal_piece] & pos) { // bishops or queens
                add_continuous_move(i, pos, diagonal_piece, occupied, 1, 1, moves, c);
                add_continuous_move(i, pos, diagonal_piece, occupied, -1, 1, moves, c);
                add_continuous_move(i, pos, diagonal_piece, occupied, -1, -1, moves, c);
                add_continuous_move(i, pos, diagonal_piece, occupied, 1, -1, moves, c);
            }
            if (const Piece straight_piece = (bitboard[turn][ROOK] & pos) ? ROOK : QUEEN; bitboard[turn][straight_piece] & pos) { // rooks or queens
                add_continuous_move(i, pos, straight_piece, occupied, 1, 0, moves, c);
                add_continuous_move(i, pos, straight_piece, occupied, 0, 1, moves, c);
                add_continuous_move(i, pos, straight_piece, occupied, -1, 0, moves, c);
                add_continuous_move(i, pos, straight_piece, occupied, 0, -1, moves, c);
            }
        }
    }
    return c;
}

// user input / initialization

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
            Piece piece = get_piece(1ULL << (8 * i + j), color);
            if (piece == NONE) {
                color = BLACK;
                piece = get_piece(1ULL << (8 * i + j), color);
            }
            std::cout << (piece == NONE ? ' ' : char(piece_map[piece] + (color ? '\0' : ' '))) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "  ";
    for (int i = 0; i < 8; i++) {
        std::cout << (char)('A' + i) << " ";
    }
    std::cout << "\n";
}

void Board::print_moves() const {
    Move moves[230];
    const int move_count = generate_moves(moves);
    for (int i = 0; i < move_count; i++) {
        moves[i].print();
    }
}