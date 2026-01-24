#include "board.h"
#include "minimax.h"
#include <limits.h>

std::optional<Move> best_move;
int root_depth;

std::optional<Move> search(const Board& board, const int depth) {
    best_move = std::nullopt;
    root_depth = depth;
    minimax(board, depth);
    return best_move;
}

int minimax(Board board, const int depth) {
    if (board.game_over() != Ongoing || depth == 0)
        return board.eval();
    int best;
    if (board.get_turn()) { // == BLACK
        best = INT_MAX;
        Move moves[MAX_MOVES];
        const int cnt = board.generate_legal_moves(moves);
        for (int i = 0; i < cnt; i++) {
            board.make_move(moves[i]);
            if (const int val = minimax(board, depth-1); val < best) {
                best = val;
                if (depth == root_depth)
                    best_move = moves[i];
            }
        }
    } else {
        best = INT_MIN;
        Move moves[MAX_MOVES];
        const int cnt = board.generate_legal_moves(moves);
        for (int i = 0; i < cnt; i++) {
            board.make_move(moves[i]);
            if (const int val = minimax(board, depth-1); val > best) {
                best = val;
                if (depth == root_depth)
                    best_move = moves[i];
            }
        }
    }
    return best;
}