#pragma once

std::optional<Move> search(const Board& board, int depth);
int minimax(Board board, int depth);