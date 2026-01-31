#include "uci.h"

#include <iostream>
#include <sstream>
#include <vector>
#include "engine.h"

std::optional<Move> parse_move(const std::string& move) {
    if (move.size() != 4 && !(move.size() == 5 && (move[4] == 'q' || move[4] == 'r' || move[4] == 'b' || move[4] == 'n')))
        return std::nullopt;
    if (!('a' <= move[0] && move[0] <= 'h' &&
        '1' <= move[1] && move[1] <= '8' &&
        'a' <= move[2] && move[2] <= 'h' &&
        '1' <= move[3] && move[3] <= '8'))
        return std::nullopt;
    const uint8_t from = static_cast<uint8_t>((move[1]-'1')*8+move[0]-'a'),
                  to   = static_cast<uint8_t>((move[3]-'1')*8+move[2]-'a');
    return std::optional(Move(from, to, NONE, NONE, REGULAR));
}

void uci_loop() {
    std::optional<std::string> fen;
    std::vector<Move> moves;
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "quit") {
            exit(0);
        } else if (token == "uci") {
            std::cout << "id name Smolchess\n" << "id author Horia B\n" << "uciok\n";
        } else if (token == "ucinewgame") {
            // nothing here for now
        } else if (token == "position") {
            iss >> token;
            if (token == "startpos") {
                fen = std::nullopt;
            } else if (token == "fen") {
                fen = "";
                for (int i = 0; i < 6; i++) {
                    iss >> token;
                    if (i != 0) *fen += " ";
                    *fen += token;
                }
            } else {
                continue;
            }
            moves.clear();
            iss >> token;
            if (token != "moves") {
                continue;
            }
            while (iss >> token) {
                std::optional<Move> move = parse_move(token);
                if (!move)
                    break;
                moves.push_back(*move);
            }
        } else if (token == "go") {
            // TODO: wtime btime winc binc
            Move best_move = Engine::search(fen, moves);
            std::cout << "bestmove " << best_move.get_string() << "\n";
        } else {
            // unknown command
        }
    }
}
