#include <cstdint>

bool is_bottom_rank(const int i) {
    return i < 8;
}
bool is_top_rank(const int i) {
    return i > 56;
}
bool is_left_file(const int i) {
    return i % 8 == 0;
}
bool is_right_file(const int i) {
    return i % 8 == 7;
}
int64_t offset(int64_t pos, const int file_cnt, const int rank_cnt) {
    if (file_cnt < 0) {
        pos >>= (-file_cnt);
    } else {
        pos <<= rank_cnt;
    }
    return pos << rank_cnt;
}
int64_t offset_file(const int64_t pos, const int cnt) {
    if (cnt >= 0) {
        return pos << cnt;
    }
    return pos >> (-cnt);
}
int64_t offset_rank(const int64_t pos, const int cnt) {
    if (cnt >= 0) {
        return pos << (8 * cnt);
    }
    return pos >> (8 * -cnt);
}