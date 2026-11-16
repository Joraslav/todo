#pragma once


#include <string>
#include <string_view>

namespace cli {

[[nodiscard]]
std::string ToLower(std::string_view str);

}  // namespace cli
