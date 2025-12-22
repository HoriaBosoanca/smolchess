#include "board.h"
#include "CLI.h"

int main() {
    CLI::setup();
    Board board;
    board.setup_normal();
    board.move_piece(1ULL << 6*8+3, 1ULL << 4*8+3);
    CLI::print_board(board);
    return 0;
}