#pragma once
#include <vector>
#include <functional>
#include <iostream>

class CommandBuffer {
public:
    // A command is simply a callable object that executes a runtime action
    using Command = std::function<void()>;

    // Record a command into the queue
    void record(Command cmd) {
        commands.push_back(cmd);
    }

    // Execute all recorded commands sequentially
    void submit() {
        std::cout << "[CommandBuffer] Submitting " << commands.size() << " commands...\n";
        for (size_t i = 0; i < commands.size(); ++i) {
            std::cout << "[CommandBuffer] Executing step " << i + 1 << "\n";
            commands[i]();
        }
        commands.clear();
        std::cout << "[CommandBuffer] Execution complete.\n";
    }

private:
    std::vector<Command> commands;
};