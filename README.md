# Smolchess

A small UCI chess bot. WIP

## Features
- UCI: partial support (sufficient for cutechess testing)
- Search: negamax + alpha-beta
- Board: basic bitboard implementation (not very tested)

## TODO
- Compatibility and bug-fixes:
    - Test and find illegal moves
      - Fix weird pawn captures and queen movements
    - Allow FEN via UCI
    - Use separate thread for input
    - Iterative deepening + time management + UCI stop command
    - Implement full UCI
- Search improvements:
    - Majority of https://www.chessprogramming.org/Search_Progression