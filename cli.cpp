#include <iostream>

#include "board.h"

bool parse_move_format(std::string& move) {
    if (move.size() < 4) {
        std::cout << "Invalid move notation!\n";
        return false;
    }
    move.resize(4);
    if ('A' <= move[0] && move[0] <= 'H')
        move[0] += ' ';
    if ('A' <= move[2] && move[2] <= 'H')
        move[2] += ' ';
    if (!('a' <= move[0] && move[0] <= 'h' &&
        '1' <= move[1] && move[1] <= '8' &&
        'a' <= move[2] && move[2] <= 'h' &&
        '1' <= move[3] && move[3] <= '8')) {
        std::cout << "Invalid move notation!\n";
        return false;
    }
    return true;
}

bool move_if_legal(Board& board, std::string& move_str) {
    if (!parse_move_format(move_str)) {
        return false;
    }
    bool found = false;
    Move my_move;
    const uint8_t from = (move_str[1]-'1')*8+move_str[0]-'a', to = (move_str[3]-'1')*8+move_str[2]-'a';
    Move moves[230];
    const int cnt = board.generate_legal_moves(moves);
    for (int i = 0; i < cnt; i++) {
        if (from == moves[i].from() && to == moves[i].to()) {
            found = true;
            my_move = moves[i];
        }
    }
    if (!found) {
        std::cout << "Invalid move!\n";
        return false;
    }
    board.make_move(my_move);
    return true;
}

void game_loop() {
    Board board;
    while (board.game_over() == Ongoing) {
        board.print_board();
        board.print_moves();
        std::string move_str;
        do {
            std::cout << (board.get_turn() ? "Black to move (ex: e7e5)\n" : "White to move (ex: e2e4)\n");
            std::getline(std::cin, move_str);
        } while (!move_if_legal(board, move_str));
    }
    std::cout << (board.game_over() == WhiteWin ? "White wins!\n" : "Black wins!\n");
}