#include <climits>
#include <algorithm>
#include <iostream>
#include "board.h"
#include "search.h"

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
        if (TURN == BLACK) {
            if (val < best) {
                best = val;
                if (depth == root_depth)
                    best_move = moves[i];
            }
            beta = std::min(beta, best);
        } else {
            if (val > best) {
                best = val;
                if (depth == root_depth)
                    best_move = moves[i];
            }
            alpha = std::max(alpha, best);
        }

        if (beta <= alpha)
            break;
    }
    return best;
}

Move search(Board& board, const int depth, int& eval) {
    root_depth = depth;
    eval = minimax(board, depth, INT_MIN, INT_MAX);
    if (!best_move) {
        std::cout << "Search failed!\n";
        exit(-1);
    }
    return *best_move;
}