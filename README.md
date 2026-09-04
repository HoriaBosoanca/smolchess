# smolchess

A small chess bot.

### features
- UCI: partial support (sufficient for cutechess testing)
- Search: negamax + alpha-beta
- Board: basic bitboard implementation (not very tested)

### config and usage
- `assets/tools` contains the cutechess GUI which can be used to play against the engine
- when compiling with CMAKE the `playtest` target can be chosen to run playtesting using cutechess-cli

### dependency links
- cutechess: https://github.com/cutechess/cutechess

### TODO
- Compatibility and bug-fixes:
    - Test and find illegal moves
      - Fix weird pawn captures and queen movements
    - Allow FEN via UCI
    - Use separate thread for input
    - Iterative deepening + time management + UCI stop command
    - Implement full UCI
- Search improvements:
    - Majority of https://www.chessprogramming.org/Search_Progression