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
        for (const Move& move : moves)
            board.make_move(move);
        int _none;
        return search(board, 4, _none);
    }
}
