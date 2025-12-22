#include <cstdint>
#include "board.h"

void Board::setup_normal() {
    for (int i = 0; i < 8; i++) {
        if (i == 0 || i == 7) {
            add_piece(1ULL << (8 * i), i % 2, ROOK);
            add_piece(1ULL << (8 * i + 1), i % 2, KNIGHT);
            add_piece(1ULL << (8 * i + 2), i % 2, BISHOP);
            add_piece(1ULL << (8 * i + 3), i % 2, KING);
            add_piece(1ULL << (8 * i + 4), i % 2, QUEEN);
            add_piece(1ULL << (8 * i + 5), i % 2, BISHOP);
            add_piece(1ULL << (8 * i + 6), i % 2, KNIGHT);
            add_piece(1ULL << (8 * i + 7), i % 2, ROOK);
        } else if (i == 1 || i == 6) {
            for (int j = 0; j < 8; j++) {
                add_piece(1ULL << (8 * i + j), i % 2 ? WHITE : BLACK, PAWN);
            }
        }
    }
}

void Board::add_piece(const uint64_t pos, const int color, const int piece) {
    bitboard[color][piece] |= pos;
}

char Board::get_piece(const uint64_t pos) const {
    for (int c = WHITE; c <= BLACK; c++)
        for (int p = PAWN; p <= KING; p++)
            if (bitboard[c][p] & pos)
                return piece_map[p] + (c == WHITE ? '\0' : ' ');
    return ' ';
}

void Board::move_piece(const uint64_t from, const uint64_t to) {
    for (int c = WHITE; c <= BLACK; c++) {
        for (int p = PAWN; p <= KING; p++) {
            if (bitboard[c][p] & from) {
                bitboard[c][p] &= ~from;
                bitboard[c][p] |= to;
            }
        }
    }
}