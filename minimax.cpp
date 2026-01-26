#include <limits.h>
#include <algorithm>
#include "board.h"
#include "minimax.h"

std::optional<Move> best_move;
int root_depth;

int minimax(Board& board, const int depth, int alpha, int beta) {
    if (board.game_over() != Ongoing || depth == 0)
        return board.eval();
    const Color TURN = board.get_turn();
    int best = TURN ? INT_MAX : INT_MIN;
    Move moves[MAX_MOVES];
    const int cnt = board.generate_legal_moves(moves);
    for (int i = 0; i < cnt; i++) {
        Board board_copy = board;
        board_copy.make_move(moves[i]);

        const int val = minimax(board_copy, depth-1, alpha, beta);
        if (TURN) {
            best = std::min(best, val);
            beta = std::min(beta, best);
        } else {
            best = std::max(best, val);
            alpha = std::max(alpha, best);
        }

        if (depth == root_depth && val == best)
            best_move = moves[i];

        if (beta <= alpha)
            break;
    }
    return best;
}

std::optional<Move> search(Board& board, const int depth, int& eval) {
    best_move = std::nullopt;
    root_depth = depth;
    eval = minimax(board, depth, INT_MIN, INT_MAX);
    return best_move;
}