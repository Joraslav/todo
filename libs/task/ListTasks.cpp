#include "ListTasks.hpp"

#include "Error.hpp"
#include "glaze/core/context.hpp"
#include "glaze/core/reflect.hpp"
#include "glaze/json/read.hpp"
#include "glaze/json/write.hpp"
#include "glaze/yaml/read.hpp"
#include "glaze/yaml/write.hpp"
#include "Result.hpp"
#include "Task.hpp"

#include <cstddef>
#include <exception>
#include <expected>
#include <filesystem>
#include <format>
#include <optional>
#include <print>
#include <string>
#include <utility>
#include <vector>

namespace task {

namespace fs = std::filesystem;
using namespace std::string_literals;

using tools::Error;
using tools::ErrorCode;
using tools::Result;

void ListTasks::AddTask(std::string name, bool done, std::optional<Date> date) {
    tasks_.push_back({
        .name = std::move(name),
        .done = done,
        .date = date,
    });
}

void ListTasks::AddTask(Task task) { tasks_.push_back(std::move(task)); }

Result<void> ListTasks::Complete(size_t number) noexcept {
    if (number < 1) {
        return std::unexpected(Error{
            .code = ErrorCode::OutOfRange,
            .message =
                std::format("Index [{}] does not contain in list", number),
        });
    }
    size_t index = number - 1;
    if (index >= tasks_.size()) {
        return std::unexpected(Error{
            .code = ErrorCode::Overflow,
            .message = "Index out of range"s,
        });
    }
    tasks_.at(index).done = true;
    return {};
}

Result<void> ListTasks::Uncomplete(size_t number) noexcept {
    if (number < 1) {
        return std::unexpected(Error{
            .code = ErrorCode::OutOfRange,
            .message =
                std::format("Index [{}] does not contain in list", number),
        });
    }
    size_t index = number - 1;
    if (index >= tasks_.size()) {
        return std::unexpected(Error{
            .code = ErrorCode::Overflow,
            .message = "Index out of range"s,
        });
    }
    tasks_.at(index).done = false;
    return {};
}

Result<void> ListTasks::SetDate(size_t index, Date date) noexcept {
    if (index >= tasks_.size()) {
        return std::unexpected(Error{
            .code = ErrorCode::OutOfRange,
            .message = "Index out of range"s,
        });
    }
    tasks_.at(index).date = date;
    return {};
}

Result<void> ListTasks::Print() const {
    try {
        if (tasks_.empty()) {
            std::println("(empty)");
        }
        for (size_t i = 0; i != tasks_.size(); ++i) {
            std::println("{:>2}. {}", i + 1, tasks_.at(i));
        }
    } catch (const std::exception& err) {
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = std::format("Error printing {}: exception: {}",
                                   ErrorCode::Unknown, err.what()),
        });
    }
    return {};
}

Result<void> ListTasks::WriteToJSON(const fs::path& file_path) const {
    std::string buffer;
    if (const auto ec = glz::write_file_json(*this, file_path.string(), buffer);
        ec) {
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = std::format("Failed to write JSON: {}",
                                   glz::format_error(ec, buffer)),
        });
    }
    return {};
}

Result<void> ListTasks::WriteToYAML(const fs::path& file_path) const {
    std::string buffer;
    if (const auto ec = glz::write_file_yaml(*this, file_path.string(), buffer);
        ec) {
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = std::format("Failed to write YAML: {}",
                                   glz::format_error(ec, buffer)),
        });
    }
    return {};
}

Result<void> ListTasks::ReadFromJSON(const fs::path& file_path) {
    std::vector<Task> temp_tasks;
    std::string buffer;
    if (const auto ec =
            glz::read_file_json(temp_tasks, file_path.string(), buffer);
        ec) {
        if (ec.ec == glz::error_code::file_open_failure) {
            return std::unexpected(Error{
                .code = ErrorCode::NotFounded,
                .message =
                    std::format("File {} not found:", file_path.string()),
            });
        }
        return std::unexpected(Error{
            .code = ErrorCode::Parse,
            .message = std::format("Failed to parse JSON: {}",
                                   glz::format_error(ec, buffer)),
        });
    }
    tasks_ = std::move(temp_tasks);
    return {};
}

Result<void> ListTasks::ReadFromYAML(const fs::path& file_path) {
    std::vector<Task> temp_tasks;
    std::string buffer;
    if (const auto ec =
            glz::read_file_yaml(temp_tasks, file_path.string(), buffer);
        ec) {
        if (ec.ec == glz::error_code::file_open_failure) {
            return std::unexpected(Error{
                .code = ErrorCode::NotFounded,
                .message =
                    std::format("File {} not found:", file_path.string()),
            });
        }
        return std::unexpected(Error{
            .code = ErrorCode::Parse,
            .message = std::format("Failed to parse YAML: {}",
                                   glz::format_error(ec, buffer)),
        });
    }
    tasks_ = std::move(temp_tasks);
    return {};
}

}  // namespace task
