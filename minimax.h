#pragma once

std::optional<Move> search(Board& board, int depth, int& eval);
int minimax(Board& board, int depth);