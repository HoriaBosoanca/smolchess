#include "uci.h"
#include "engine.h"
#include <iostream>
#include <sstream>
#include <vector>

static Move get_move_from_token(const std::string& move) {
    if(!(move.size() == 4 || (move.size() == 5 && (move[4] == 'q' || move[4] == 'r' || move[4] == 'b' || move[4] == 'n'))) {
        throw std::runtime_error("bad move input format!");
    }
    if (!('a' <= move[0] && move[0] <= 'h' &&
        '1' <= move[1] && move[1] <= '8' &&
        'a' <= move[2] && move[2] <= 'h' &&
        '1' <= move[3] && move[3] <= '8')) {
        throw std::runtime_error("bad move input format #2!");
    }
    const uint8_t from = static_cast<uint8_t>((move[1]-'1')*8+move[0]-'a'),
                  to   = static_cast<uint8_t>((move[3]-'1')*8+move[2]-'a');
    MoveType mt = REGULAR;
    if (move.size() == 5) {
        switch (move[4]) {
            case 'q': mt = QUEEN_PROMOTION; break;
            case 'r': mt = ROOK_PROMOTION; break;
            case 'b': mt = BISHOP_PROMOTION; break;
            case 'n': mt = KNIGHT_PROMOTION; break;
            default: throw std::runtime_error("bad promotion input format!");
        }
    }
    return Move(from, to, NONE, NONE, mt);
}

void run() {
    std::optional<std::string> fen;
    std::vector<Move> moves;
    while (true) {
        // read the current input line
        std::string _input_line;
        std::getline(std::cin, _input_line);
        std::istringstream input_line(_input_line);
        // read first the word of the input line
        std::string token;
        input_line >> token;
        if (token == "quit") {
            break;
        }
        if (token == "uci") {
            std::cout << "id name smolchess\nid author Horia B\nuciok\n";
        } else if (token == "ucinewgame") {
            // no need to handle since position command is required anyway before go
        } else if (token == "isready") {
            std::cout << "readyok\n";
        } else if (token == "position") {
            // position [fen <fenstring> | startpos ]  moves <move1> .... <movei>
            // clear position data
            fen = std::nullopt;
            moves.clear();
            // handle fen if any
            input_line >> token;
            if (token == "fen") {
                fen = "";
                for (int i = 0; i < 6; i++) {
                    if (i != 0) *fen += " ";
                    input_line >> token;
                    fen.value() += token;
                }
            } else if (token != "startpos") throw std::runtime_error("bad position command format!");
            if (input_line >> token) {
                if (token != "moves")
                    throw std::runtime_error("bad position command format #2!");
                while (input_line >> token)
                    moves.push_back(get_move_from_token(token));
            }
        } else if (token == "go") {
            // time controls (wtime btime winc binc) to be implemented
            std::cout << "bestmove " << bestmove(fen, moves).get_string() << "\n";
        } else {
            // unknown command
        }
    }
}
