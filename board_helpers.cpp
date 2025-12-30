#include <cstdint>
#include <iostream>
#include "board.h"

int rank(const int8_t i) {
    return i/8+1;
}
char file(const int8_t i) {
    return (char)(i%8+'a');
}
uint8_t offset_idx(int8_t i, const int file_cnt, const int rank_cnt) {
    i += file_cnt;
    i += rank_cnt*8;
    return i;
}
uint64_t offset_pos(uint64_t pos, const int file_cnt, const int rank_cnt) {
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
bool parse_move_format(std::string& move) {
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
void print_piece(const char piece) {
    switch (piece) {
    case 'p': std::cout << "♙"; break;
    case 'n': std::cout << "♘"; break;
    case 'b': std::cout << "♗"; break;
    case 'r': std::cout << "♖"; break;
    case 'q': std::cout << "♕"; break;
    case 'k': std::cout << "♔"; break;
    case 'P': std::cout << "♟"; break;
    case 'N': std::cout << "♞"; break;
    case 'B': std::cout << "♝"; break;
    case 'R': std::cout << "♜"; break;
    case 'Q': std::cout << "♛"; break;
    case 'K': std::cout << "♚"; break;
    default: std::cout << "　"; // special Unicode space the size of a chess piece
    }
}