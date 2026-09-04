#pragma once

#include "board.h"
#include <vector>

Move bestmove(const std::optional<std::string>& fen, const std::vector<Move>& moves);