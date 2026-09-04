#include "engine.h"
#include "board.h"
#include "search.h"
#include <stdexcept>
#include <vector>

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