#pragma once

#include "magic_enum/magic_enum.hpp"

#include <cstdint>
#include <format>
#include <string>
#include <string_view>

namespace tools {

enum class ErrorCode : uint8_t {
    NotFounded,
    OutOfRange,
    Parse,
    Overflow,
    Inserted,
    Unknown,
};

struct Error final {
    ErrorCode code;
    std::string message;
};

}  // namespace tools

template <>
struct std::formatter<tools::ErrorCode> : std::formatter<std::string_view> {
    auto format(tools::ErrorCode ec, std::format_context& ctx) const {
        const auto name = magic_enum::enum_name(ec);
        if (name.empty()) {
            return std::formatter<std::string_view>::format("Unknown", ctx);
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<tools::Error> {
    static constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    static auto format(const tools::Error& err, std::format_context& ctx) {
        return std::format_to(ctx.out(), "[{}]: {}", err.code, err.message);
    }
};
