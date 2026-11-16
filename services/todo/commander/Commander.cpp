#include "Commander.hpp"

#include "Error.hpp"
#include "Result.hpp"
#include "Task.hpp"

#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <optional>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

using namespace std::string_literals;
using namespace std::string_view_literals;

using tools::Error;
using tools::ErrorCode;
using tools::Result;

namespace {

[[nodiscard]]
Result<size_t> ParseIndex(std::string_view str) noexcept {
    size_t index{};
    const auto [ptr, ec] =
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        std::from_chars(str.data(), str.data() + str.size(), index);
    if (ec != std::errc{}) {
        return std::unexpected(Error{
            .code = ErrorCode::Parse,
            .message = "Parsing inner error"s,
        });
    }
    if (ptr != str.cend()) {
        return std::unexpected(Error{
            .code = ErrorCode::Parse,
            .message = "Invalid characters in index"s,
        });
    }
    return index;
}

[[nodiscard]]
Result<task::Date> ParseMinute(std::string_view minutes_str) {
    uint32_t minutes{};
    const auto [ptr, ec] = std::from_chars(
        minutes_str.data(),
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        minutes_str.data() + minutes_str.size(), minutes);
    if (ec != std::errc{}) {
        return std::unexpected(Error{
            .code = ErrorCode::Parse,
            .message = "Parsing inner error"s,
        });
    }
    if (ptr != minutes_str.cend()) {
        return std::unexpected(Error{
            .code = ErrorCode::Parse,
            .message = "Invalid characters in time"s,
        });
    }
    return std::chrono::floor<std::chrono::seconds>(
        std::chrono::system_clock::now() + std::chrono::minutes(minutes));
}

[[nodiscard]]
Result<task::Date> ParseDate(std::string_view date_str) {
    std::istringstream stream{date_str};
    task::Date date;
    if (stream >> std::chrono::parse("%F %T", date)) {
        if (stream.eof()) {
            return date;
        }
    }
    return std::unexpected(Error{
        .code = ErrorCode::Parse,
        .message = std::format("Error parsed date: {}", date_str),
    });
}

[[nodiscard]]
std::string ConcatinateWords(std::span<const std::string> words) {
    return words | std::views::join_with(' ') | std::ranges::to<std::string>();
}

[[nodiscard]]
Result<task::Task> ParseTask(std::span<const std::string> words) {
    using namespace std::chrono;
    if (words.empty()) [[unlikely]] {
        return std::unexpected(Error{
            .code = ErrorCode::Parse,
            .message = "Task name is required"s,
        });
    }

    std::optional<task::Date> date;
    const std::string& last = words.back();
    if (last.front() == ':') {
        const auto minutes_res = ParseMinute(last.subview(1));
        if (!minutes_res) {
            return std::unexpected(minutes_res.error());
        }
        date = *minutes_res;
    }
    if (last.front() == '!') {
        const auto date_res = ParseDate(last.subview(1));
        if (!date_res) {
            return std::unexpected(date_res.error());
        }
        date = *date_res;
    }

    std::string task_name{ConcatinateWords(words.subspan(0, words.size() - 1))};
    task::Task task{
        .name = std::move(task_name),
        .done = false,
        .date = date,
    };
    return task;
}

}  // namespace

namespace todo {

////////// Private methods //////////
Result<void> Commander::AddTaskHandle(task::Task task) {
    const auto before_size = list_->Size();
    list_->AddTask(std::move(task));
    if (before_size >= list_->Size()) {
        return std::unexpected(Error{
            .code = ErrorCode::Inserted,
            .message = "Add new task error"s,
        });
    }
    return {};
}

Result<void> Commander::MarkComplete(size_t task_index) noexcept {
    return list_->Complete(task_index);
}

Result<void> Commander::MarkUncomplete(size_t task_index) noexcept {
    return list_->Uncomplete(task_index);
}

Result<void> Commander::MakeMarkHandle(std::string_view index_str,
                                       MarkType mark_type) noexcept {
    const auto index_result = ParseIndex(index_str);
    if (!index_result) {
        return std::unexpected(index_result.error());
    }
    switch (mark_type) {
        case MarkType::Mark:
            return MarkComplete(*index_result);
            break;
        case MarkType::Unmark:
            return MarkUncomplete(*index_result);
            break;
    }
    return {};
}

Result<void> Commander::PrintTasksHandle() { return list_->Print(); }

////////// Public methods //////////
Result<void> Commander::Handle(std::span<const std::string> commands) {
    if (commands.empty()) {
        return std::unexpected(Error{
            .code = ErrorCode::Parse,
            .message = "No Command"s,
        });
    }
    const auto& base_command = commands.front();
    if (base_command == "list"sv) {
        if (commands.size() != 1) {
            return std::unexpected(Error{
                .code = ErrorCode::Overflow,
                .message = "Too mach arguments for command `list`"s,
            });
        }
        return PrintTasksHandle();
    }
    if (base_command == "mark"sv) {
        if (commands.size() < 2) {
            return std::unexpected(Error{
                .code = ErrorCode::Parse,
                .message = "Missing argument"s,
            });
        }
        return MakeMarkHandle(commands.at(1), MarkType::Mark);
    }
    if (base_command == "unmark"sv) {
        if (commands.size() < 2) {
            return std::unexpected(Error{
                .code = ErrorCode::Parse,
                .message = "Missing argument"s,
            });
        }
        return MakeMarkHandle(commands.at(1), MarkType::Unmark);
    }
    if (base_command == "add"sv) {
        if (commands.size() < 2) {
            return std::unexpected(Error{
                .code = ErrorCode::Parse,
                .message = "Missing argument"s,
            });
        }
        auto task_result = ParseTask(commands.subspan(1));
        if (!task_result) {
            return std::unexpected(task_result.error());
        }
        return AddTaskHandle(std::move(*task_result));
    }
    return std::unexpected(Error{
        .code = ErrorCode::Parse,
        .message = "Unknown command"s,
    });
}

}  // namespace todo
