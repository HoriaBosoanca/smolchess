#include <cstdint>
#include <iostream>
#include "board.h"

#include <bitset>

#include "board_helpers.h"

void Board::setup_normal() {
    for (int i = 0; i < 8; i++) {
        if (i == 0 || i == 7) {
            add_piece(1ULL << (8 * i    ), i % 2 ? WHITE : BLACK, ROOK);
            add_piece(1ULL << (8 * i + 1), i % 2 ? WHITE : BLACK, KNIGHT);
            add_piece(1ULL << (8 * i + 2), i % 2 ? WHITE : BLACK, BISHOP);
            add_piece(1ULL << (8 * i + 3), i % 2 ? WHITE : BLACK, QUEEN);
            add_piece(1ULL << (8 * i + 4), i % 2 ? WHITE : BLACK, KING);
            add_piece(1ULL << (8 * i + 5), i % 2 ? WHITE : BLACK, BISHOP);
            add_piece(1ULL << (8 * i + 6), i % 2 ? WHITE : BLACK, KNIGHT);
            add_piece(1ULL << (8 * i + 7), i % 2 ? WHITE : BLACK, ROOK);
        } else if (i == 1 || i == 6) {
            for (int j = 0; j < 8; j++) {
                add_piece(1ULL << (8 * i + j), i % 2 ? WHITE : BLACK, PAWN);
            }
        }
    }
}

void Board::add_piece(const uint64_t pos, const Color color, const int piece) {
    bitboard[color][piece] |= pos;
}

bool Board::parse_move_format(std::string& move) {
    move.resize(4);
    if ('A' <= move[0] && move[0] <= 'H')
        move[0] += ' ';
    if ('A' <= move[2] && move[2] <= 'H')
        move[2] += ' ';
    if (!('a' <= move[0] && move[0] <= 'h' &&
        '1' <= move[1] && move[1] <= '8' &&
        'a' <= move[2] && move[2] <= 'h' &&
        '1' <= move[3] && move[3] <= '8')) {
        std::cout << "Invalid move notation!\n";
        return false;
    }
    return true;
}

bool Board::move_unknown_str(std::string& move) {
    if (!parse_move_format(move)) {
        return false;
    }
    move_unknown(1ULL << ((move[1]-'1')*8+move[0]-'a'), 1ULL << ((move[3]-'1')*8+move[2]-'a'));
    return true;
}

char Board::get_piece(const uint64_t pos) const {
    for (int c = WHITE; c <= BLACK; c++)
        for (int p = PAWN; p <= KING; p++)
            if (bitboard[c][p] & pos)
                return piece_map[p] + (c == WHITE ? '\0' : ' ');
    return ' ';
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

/*
<<1 => next file
>>1 => previous file
<<8 => next rank
>>8 => previous rank
*/
void Board::generate_moves(const Color color) {
    const uint64_t white_occupied = get_occupied(WHITE), black_occupied = get_occupied(BLACK);
    const uint64_t occupied[2] = {white_occupied, black_occupied};
    const uint64_t any_occupied = white_occupied | black_occupied;

    for (uint8_t i = 0; i < 64; i++) {
        const uint64_t pos = 1ULL << i;
        // pawns
        if (bitboard[color][PAWN] & pos) {
            if (color == WHITE) {
                if (top_rank(i)) { // if it's rank 8
                    std::cout << "promote\n";
                    continue;
                }
                if (!(offset_pos(pos, 0, 1) & any_occupied)) { // if it has space ahead
                    add_move(Move(i, offset_idx(i, 0, 1), PAWN), color);
                }
                if (!l_file(i) && (offset_pos(pos, -1, 1) & occupied[BLACK])) { // if it has a piece ahead-left
                    add_move(Move(i, offset_idx(i, -1, 1), PAWN), color);
                }
                if (!r_file(i) && (offset_pos(pos, 1, 1) & occupied[BLACK])) { // if it has a piece ahead-right
                    add_move(Move(i, offset_idx(i, 1, 1), PAWN), color);
                }
            } else {

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
    const int mask = 0b111111;
    const int from = move & mask;
    const int to = (move >> 6) & mask;
    const int piece = (move >> 12) & mask;
    std::cout << piece_map[piece] << " " << (char)(from%8+'a') << from/8+1 << " " << (char)(to%8+'a') << to/8+1 << " " << "\n";
}
