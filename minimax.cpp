#include "board.h"
#include "minimax.h"
#include <limits.h>

std::optional<Move> best_move;
int root_depth;

std::optional<Move> search(Board& board, const int depth, int& eval) {
    best_move = std::nullopt;
    root_depth = depth;
    eval = minimax(board, depth);
    return best_move;
}

int minimax(Board& board, const int depth) {
    if (board.game_over() != Ongoing || depth == 0)
        return board.eval();
    int best = board.get_turn() ? INT_MAX : INT_MIN;
    Move moves[MAX_MOVES];
    const int cnt = board.generate_legal_moves(moves);
    for (int i = 0; i < cnt; i++) {
        Board board_copy = board;
        board_copy.make_move(moves[i]);
        if (const int val = minimax(board_copy, depth-1); board.get_turn() ? val < best : val > best) {
            best = val;
            if (depth == root_depth)
                best_move = moves[i];
        }
    }
    return best;
}