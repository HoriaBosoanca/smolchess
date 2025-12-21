#include <cstdint>
#include <iostream>
#include <windows.h>

#include "board.h"

namespace CLI {
    void setup() {
        SetConsoleOutputCP(CP_UTF8);
    }
    void print_piece(const char piece) {
        switch (piece) {
        case 'p': {
            std::cout << "♙";
            break;
        }
        case 'n': {
            std::cout << "♘";
            break;
        }
        case 'b': {
            std::cout << "♗";
            break;
        }
        case 'r': {
            std::cout << "♖";
            break;
        }
        case 'q': {
            std::cout << "♕";
            break;
        }
        case 'k': {
            std::cout << "♔";
            break;
        }
        case 'P': {
            std::cout << "♟";
            break;
        }
        case 'N': {
            std::cout << "♞";
            break;
        }
        case 'B': {
            std::cout << "♝";
            break;
        }
        case 'R': {
            std::cout << "♜";
            break;
        }
        case 'Q': {
            std::cout << "♛";
            break;
        }
        case 'K': {
            std::cout << "♚";
            break;
        }
        default: {
            std::cout << "　"; // special unicode space the size of a chess piece
        }
        }
    }
    void print_board(const Board& board) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                print_piece(board.get_piece(1ULL << (8 * i + j)));
                std::cout << " ";
            }
            std::cout << "\n";
        }
    }
    void await_move() {

    }
}
