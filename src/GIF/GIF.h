#pragma once

#include <string>
#include <vector>

#include "Type.h"

namespace GIF {

std::vector<std::vector<Frame>> load(const std::vector<std::string> &filePaths);

} // namespace GIF
