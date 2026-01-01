#pragma once

inline int rank(const uint8_t i) {
    return i/8+1;
}
inline char file(const uint8_t i) {
    return (char)(i%8+'a');
}
inline uint8_t offset_idx(uint8_t i, const int file_cnt, const int rank_cnt) {
    i += file_cnt;
    i += rank_cnt*8;
    return i;
}
inline uint64_t offset_pos(uint64_t pos, const int file_cnt, const int rank_cnt) {
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

bool parse_move_format(std::string& move);
bool continuous_move_condition(uint8_t i, int file_offset, int rank_offset, int file_increase, int rank_increase);
void print_piece(char piece);