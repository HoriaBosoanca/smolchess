#include "search.h"
#include "board.h"
#include <climits>
#include <algorithm>
#include <iostream>
#include <stdexcept>

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
        throw std::runtime_error("search failed!");
    }
    return best_move.value();
}

Move bestmove(const std::optional<std::string>& fen, const std::vector<Move>& moves) {
    Board board(fen);
    for (const Move& move : moves) {
        Move generated_moves[MAX_MOVES];
        const int move_cnt = board.generate_legal_moves(generated_moves);
        bool found_move = false;
        for (int i = 0; i < move_cnt; i++)
            if (generated_moves[i] == move) {
                found_move = true;
                board.make_move(move);
                break;
            }
        if (!found_move) {
            throw std::runtime_error("no legal move matching input found!");
        }
    }
    int _nothing;
    return search(board, 4, _nothing);
}