#include <cstdint>
#include <iostream>
#include "board.h"
#include "board_helpers.h"

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