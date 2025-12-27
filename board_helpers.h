#pragma once

bool bottom_rank(int i);
bool top_rank(int i);
bool l_file(int i);
bool r_file(int i);
uint8_t offset_idx(int8_t idx, int file_cnt, int rank_cnt);
uint64_t offset_pos(uint64_t pos, int file_cnt, int rank_cnt);