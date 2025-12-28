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