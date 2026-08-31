#pragma once

#include <optional>
#include <string>
#include <vector>
#include "board.h"

namespace Engine {
    void loop();
    Move search(const std::optional<std::string>& fen, const std::vector<Move>& moves);
}