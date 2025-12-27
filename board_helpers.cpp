#include <cstdint>

bool bottom_rank(const int i) {
    return i < 8;
}
bool top_rank(const int i) {
    return i > 56;
}
bool l_file(const int i) {
    return i % 8 == 0;
}
bool r_file(const int i) {
    return i % 8 == 7;
}
uint8_t offset_idx(int8_t idx, const int file_cnt, const int rank_cnt) {
    idx += file_cnt;
    idx += rank_cnt*8;
    return idx;
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
