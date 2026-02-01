#include <vector>
#include "engine.h"
#include "board.h"
#include "cli.h"
#include "search.h"
#include "uci.h"

#define USE_UCI true

namespace Engine {
    void loop() {
        if (USE_UCI) {
            uci_loop();
        } else {
            cli_loop();
        }
    }

    Move search(const std::optional<std::string>& fen, const std::vector<Move>& moves) {
        Board board = fen ? Board(fen) : Board();
        Move gen_moves[MAX_MOVES];
        for (const Move& move : moves) {
            const int cnt = board.generate_legal_moves(gen_moves);
            for (int i = 0; i < cnt; i++)
                if (gen_moves[i].from() == move.from() && gen_moves[i].to() == move.to()) {
                    if (move.move_type() != REGULAR) { // if the move has a promotion
                        if (move.move_type() == gen_moves[i].move_type()) { // pick the right one
                            board.make_move(gen_moves[i]);
                        }
                    } else {
                        board.make_move(gen_moves[i]);
                        break;
                    }
                }
        }
        int _none;
        return search(board, 4, _none);
    }
}
