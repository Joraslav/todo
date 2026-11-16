#include "Cli.hpp"
#include "commander/Commander.hpp"
#include "Config.hpp"
#include "ListTasks.hpp"

#include <algorithm>
#include <exception>
#include <filesystem>
#include <iterator>
#include <optional>
#include <print>
#include <span>
#include <string>
#include <vector>

using namespace task;
using namespace tools;
using namespace todo;
using namespace std::string_literals;

namespace {

std::vector<std::string> ParseCommands(std::span<char*> args) {
    const std::span<char*> sub_args = args.subspan(1);
    std::vector<std::string> commands;
    commands.reserve(sub_args.size());
    std::ranges::transform(sub_args, std::back_inserter(commands),
                           [](const char* s) { return cli::ToLower(s); });
    return commands;
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::vector<std::string> commands =
        ParseCommands(std::span(argv, argc));
    try {
        const std::filesystem::path test_dir{"tests/config.json"};
        ListTasks list;
        ConfigManager cfg_manager(test_dir);
        const auto load_tasks_res =
            list.ReadFromJSON(cfg_manager.GetStoragePath());
        if (!load_tasks_res) {
            std::println("Error load tasks: {}", load_tasks_res.error());
        }
        Commander commander(&list, &cfg_manager);

        for (const auto& command : commands) {
            std::println("command: {}", command);
        }

        const std::filesystem::path storage_path = cfg_manager.GetStoragePath();
        std::println("Storage path: {}", storage_path.string());

        const auto command_result = commander.Handle(commands);
        if (!command_result) {
            std::println("{}", command_result.error());
        }
    } catch (const std::exception& e) {
        std::println("Error catch: {}", e.what());
    }
    return 0;
}
