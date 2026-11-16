#include "Cli.hpp"

#include <ranges>
#include <string>
#include <string_view>

namespace cli {

std::string ToLower(std::string_view str) {
    return str | std::views::transform([](char c) constexpr {
               return (c >= 'A' && c <= 'Z')
                          ? static_cast<char>(c + ('a' - 'A'))
                          : c;
           }) |
           std::ranges::to<std::string>();
}

}// namespace cli
