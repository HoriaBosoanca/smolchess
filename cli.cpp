#include <fstream>
#include <iostream>
#include <unordered_map>
#include "board.h"
#include "search.h"

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

std::unordered_map<char, MoveType> promotion_map = {
    {'k', KNIGHT_PROMOTION},
    {'b', BISHOP_PROMOTION},
    {'r', ROOK_PROMOTION},
    {'q', QUEEN_PROMOTION}
};
bool move_if_legal(Board& board, std::string& move_str) {
    if (!parse_move_format(move_str)) {
        return false;
    }
    std::optional<Move> my_move;
    std::optional<MoveType> promotion_type;
    const auto from = static_cast<uint8_t>((move_str[1]-'1')*8+move_str[0]-'a'),
    to = static_cast<uint8_t>((move_str[3]-'1')*8+move_str[2]-'a');
    Move moves[MAX_MOVES];
    const int cnt = board.generate_legal_moves(moves);
    for (int i = 0; i < cnt; i++) {
        if (from == moves[i].from() && to == moves[i].to()) {
            if (!promotion_type && moves[i].move_type() >= KNIGHT_PROMOTION) {
                while (true) {
                    std::cout << "Select promotion piece (ex: q)\n";
                    std::string pt;
                    std::getline(std::cin, pt);
                    pt.resize(1);
                    if (pt[0] != 'k' && pt[0] != 'b' && pt[0] != 'r' && pt[0] != 'q') {
                        std::cout << "Invalid promotion piece!\n";
                    } else {
                        promotion_type = promotion_map[pt[0]];
                        break;
                    }
                }
            }
            if (promotion_type && promotion_type != moves[i].move_type())
                continue;
            my_move = moves[i];
        }
    }
    if (!my_move) {
        std::cout << "Invalid move!\n";
        return false;
    }
    board.make_move(*my_move);
    return true;
}

#define BOT_TURN BLACK
#define NO_FILE ""
#define K_CASTLE "assets/tests/k_castle.txt"
#define KK_CHECK "assets/tests/kk_check.txt"
#define Q_CASTLE "assets/tests/q_castle.txt"
#define PROMOTION1 "assets/tests/promotion1.txt"
#define PROMOTION2 "assets/tests/promotion2.txt"
#define KID "assets/openings/kid.txt"

void cli_loop() {
    std::ifstream in(NO_FILE);
    Board board;
    while (board.game_over() == Ongoing) {
        std::optional<std::string> file_move = "";
        if (!std::getline(in, *file_move))
            file_move = std::nullopt;
        if (board.get_turn() != BOT_TURN || file_move) {
            board.print_board();
            board.print_moves();
            std::string cin_move;
            do {
                if (!file_move) {
                    std::cout << (!BOT_TURN ? "Black to move (ex: e7e5)\n" : "White to move (ex: e2e4)\n");
                    std::getline(std::cin, cin_move);
                }
            } while (!move_if_legal(board, file_move ? *file_move : cin_move));
        } else {
            int score = 0;
            const Move best_move = search(board, 4, score);
            board.make_move(best_move);
            std::cout << "Game has an eval of " << (static_cast<double>(score)/100.0) << " for white\n";
        }
    }
    std::cout << (board.game_over() == WhiteWin ? "White wins!\n" : "Black wins!\n");
}