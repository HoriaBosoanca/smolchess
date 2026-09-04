#pragma once

#include <optional>
#include <string>
#include <vector>
#include "board.h"

Move bestmove(const std::optional<std::string>& fen, const std::vector<Move>& moves);