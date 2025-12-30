#include <iostream>
#include "board.h"

[[noreturn]] int main() {
    Board board;
    do {
        board.generate_moves();
        board.print_board();
        board.print_moves();
        std::string move;
        do {
            std::cout << (board.get_turn() == WHITE ? "White" : "Black") << " to move (ex: e2e4)\n";
            std::getline(std::cin, move);
        } while (!board.make_move_str(move));
    } while (true);
    return 0;
}