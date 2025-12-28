#include <iostream>

#include "board.h"
#include "CLI.h"

int main() {
    CLI::setup();
    Board board;
    board.setup_normal();
    Color color = WHITE;
    do {
        CLI::print_board(board);
        board.generate_moves(color);
        board.print_moves(color);
        CLI::await_move(board, color);
        color = (Color)!color;
    } while (true);
    return 0;
}