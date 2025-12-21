#include <cstdint>
#include <iostream>
#include "board.h"

void Board::setup_normal() {
    for (int i = 0; i < 8; i++) {
        if (i == 0 || i == 7) {
            const char color_char_offset = (i == 0) ? '\0' : ' ';
            add_piece(1ULL << (8 * i), 'r' - color_char_offset);
            add_piece(1ULL << (8 * i + 1), 'n' - color_char_offset);
            add_piece(1ULL << (8 * i + 2), 'b' - color_char_offset);
            add_piece(1ULL << (8 * i + 3), 'k' - color_char_offset);
            add_piece(1ULL << (8 * i + 4), 'q' - color_char_offset);
            add_piece(1ULL << (8 * i + 5), 'b' - color_char_offset);
            add_piece(1ULL << (8 * i + 6), 'n' - color_char_offset);
            add_piece(1ULL << (8 * i + 7), 'r' - color_char_offset);
        } else if (i == 1 || i == 6) {
            const char color_char_offset = ((i == 1) ? '\0' : ' ');
            for (int j = 0; j < 8; j++) {
                add_piece(1ULL << (8 * i + j), 'p' - color_char_offset);
            }
        }
    }
}

void Board::add_piece(const uint64_t pos, const char piece) {
    if (piece >= 'a') {
        white |= pos;
    } else {
        black |= pos;
    }
    switch (piece >= 'a' ? piece : piece + ' ') {
    case 'p': {
        pawn |= pos;
        break;
    }
    case 'n': {
        knight |= pos;
        break;
    }
    case 'b': {
        bishop |= pos;
        break;
    }
    case 'r': {
        rook |= pos;
        break;
    }
    case 'q': {
        queen |= pos;
        break;
    }
    case 'k': {
        king |= pos;
        break;
    }
    default: {
        std::cout << "Invalid piece!\n";
        exit(-1);
    }
    }
}

char Board::get_piece(const uint64_t pos) const {
    const char color_char_offset = (white & pos) ? '\0' : ' ';
    if (pawn & pos) {
        return 'p' - color_char_offset;
    }
    if (knight & pos) {
        return 'n' - color_char_offset;
    }
    if (bishop & pos) {
        return 'b' - color_char_offset;
    }
    if (rook & pos) {
        return 'r' - color_char_offset;
    }
    if (queen & pos) {
        return 'q' - color_char_offset;
    }
    if (king & pos) {
        return 'k' - color_char_offset;
    }
    return ' ';
}

void Board::move_piece(const uint64_t from, const uint64_t to) {
    if (white & from) { // if the piece to move is white
        white &= ~from; // clear previous square
        white |= to; // occupy new square
    } else if (black & from) {
        black &= ~from;
        black |= to;
    }
    if (pawn & from) {
        pawn &= ~from;
        pawn |= to;
    } else if (knight & from) {
        knight &= ~from;
        knight |= to;
    } else if (bishop & from) {
        bishop &= ~from;
        bishop |= to;
    } else if (rook & from) {
        rook &= ~from;
        rook |= to;
    } else if (queen & from) {
        queen &= ~from;
        queen |= to;
    } else if (king & from) {
        king &= ~from;
        king |= to;
    }
}