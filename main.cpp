#include <iostream>

#include "board.h"
#include "CLI.h"

int main() {
    CLI::setup();
    Board board;
    board.setup_normal();
    std::string move = "e2e4";
    board.move_unknown_str(move);
    CLI::print_board(board);
    board.generate_moves(WHITE);
    board.print_moves(WHITE);
    while (true) {
        CLI::await_move(board);
        CLI::print_board(board);
    }
    return 0;
}