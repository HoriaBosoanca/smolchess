#pragma once

bool is_bottom_rank(int i);
bool is_top_rank(int i);
bool is_left_file(int i);
bool is_right_file(int i);
int64_t offset_file(int64_t pos, int cnt);
int64_t offset_rank(int64_t pos, int cnt);