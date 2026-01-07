#pragma once
#include <chrono>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>

struct AutoSaveSettings {
    bool enableManualTrigger = true;
};

class AutoSaveManager {
public:
    static AutoSaveManager& instance();
    void init();
    void shutdown();
    bool triggerManualSave();
    std::vector<std::string> getLogs();
    AutoSaveSettings settings;
private:
    AutoSaveManager();
    ~AutoSaveManager();
    bool invokeGameSaveByUISimulation();
    void log(const std::string& s);
    std::vector<std::string> m_logs;
    std::mutex m_mtx;
    std::atomic<bool> m_running{false};
};

