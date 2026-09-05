# smolchess

A small chess bot.

### features
- UCI: partial support (sufficient for cutechess testing)
- Search: Negamax + alpha-beta

### config and usage
- `assets/tools` contains the cutechess GUI which can be used to play against the engine
- when compiling with CMAKE, the `playtest` target can be chosen to run playtesting using cutechess-cli

### dependency links
- cutechess: https://github.com/cutechess/cutechess

### TODO
- Test the bitboard more and find illegal moves
- Use separate thread for input + iterative deepening + UCI time controls and stop command
- Use an opening book
- Teach engine to avoid draw by 3-fold repetition and stalemate
- Implement more/full UCI
- Search improvements: the majority of https://www.chessprogramming.org/Search_Progression
- Allow FEN via UCI