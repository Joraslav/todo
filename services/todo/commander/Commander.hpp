#pragma once

#include "Config.hpp"
#include "ListTasks.hpp"
#include "Result.hpp"
#include "Task.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

namespace todo {

enum class MarkType : uint8_t {
    Mark,
    Unmark,
};

class Commander final {
 public:
    explicit Commander(task::ListTasks* list_of_tasks,
                       tools::ConfigManager* config_manager)
        : list_(list_of_tasks), config_(config_manager) {
        if (list_ == nullptr) [[unlikely]] {
            throw std::invalid_argument("ListTasks is null pointer");
        }
        if (config_ == nullptr) [[unlikely]] {
            throw std::invalid_argument("ConfigManager is null pointer");
        }
    }

    [[nodiscard]]
    tools::Result<void> Handle(std::span<const std::string> commands);

 private:
    [[nodiscard]]
    tools::Result<void> AddTaskHandle(task::Task task);
    [[nodiscard]]
    tools::Result<void> PrintTasksHandle();
    [[nodiscard]]
    tools::Result<void> MakeMarkHandle(std::string_view index_str,
                                       MarkType mark_type) noexcept;
    [[nodiscard]]
    tools::Result<void> MarkComplete(size_t task_index) noexcept;
    [[nodiscard]]
    tools::Result<void> MarkUncomplete(size_t task_index) noexcept;

    task::ListTasks* list_;
    tools::ConfigManager* config_;
};

}  // namespace todo
