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
            case 'p': std::cout << "♙"; break;
            case 'n': std::cout << "♘"; break;
            case 'b': std::cout << "♗"; break;
            case 'r': std::cout << "♖"; break;
            case 'q': std::cout << "♕"; break;
            case 'k': std::cout << "♔"; break;
            case 'P': std::cout << "♟"; break;
            case 'N': std::cout << "♞"; break;
            case 'B': std::cout << "♝"; break;
            case 'R': std::cout << "♜"; break;
            case 'Q': std::cout << "♛"; break;
            case 'K': std::cout << "♚"; break;
            default: std::cout << "　"; // special Unicode space the size of a chess piece
        }
    }

    void print_board(const Board& board) {
        for (int i = 0; i < 8; i++) {
            std::cout << 8-i << " ";
            for (int j = 0; j < 8; j++) {
                print_piece(board.get_piece(1ULL << (8 * i + j)));
                std::cout << " ";
            }
            std::cout << "\n";
        }
        std::cout << "  ";
        for (int i = 0; i < 8; i++) {
            std::cout << (char)('A' + i);
            print_piece('\0');
        }
    }

    void handle_notation(char* move) {
        if ('A' <= move[0] && move[0] <= 'H')
            move[0] += ' ';
        if ('A' <= move[2] && move[2] <= 'H')
            move[2] += ' ';
        if (!('a' <= move[0] && move[0] <= 'h' &&
            '1' <= move[1] && move[1] <= '8' &&
            'a' <= move[2] && move[2] <= 'h' &&
            '1' <= move[3] && move[3] <= '8')) {
            std::cout << "Invalid move notation!\n";
        }
    }

    void await_move(Board& board) {
        char move[4];
        std::cin >> move;
        handle_notation(move);

    }
}
