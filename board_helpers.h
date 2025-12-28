#pragma once

int rank(int8_t i);
char file(int8_t i);
uint8_t offset_idx(int8_t i, int file_cnt, int rank_cnt);
uint64_t offset_pos(uint64_t pos, int file_cnt, int rank_cnt);

bool parse_move_format(std::string& move);