#include <iostream>
#include "board.h"
#ifdef _WIN32
#include <windows.h>
#endif

[[noreturn]] int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    Board board;
    do {
        board.generate_moves();
        board.print_board();
        board.print_moves();
        std::string move;
        do {
            std::cout << (board.get_turn() == WHITE ? "White" : "Black") << " to move:\n";
            std::getline(std::cin, move);
        } while (!board.make_move_str(move));
    } while (true);
    return 0;
}