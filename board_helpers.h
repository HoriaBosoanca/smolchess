#pragma once

int rank(uint8_t i);
char file(uint8_t i);
uint8_t offset_idx(uint8_t i, int file_cnt, int rank_cnt);
uint64_t offset_pos(uint64_t pos, int file_cnt, int rank_cnt);

bool parse_move_format(std::string& move);
bool continuous_move_condition(uint8_t i, int file_offset, int rank_offset, int file_increase, int rank_increase);
void print_piece(char piece);