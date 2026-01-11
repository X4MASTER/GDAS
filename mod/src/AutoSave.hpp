#pragma once
#include <Geode/Geode.hpp>
#include <chrono>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>
#include <functional>

using namespace geode::prelude;

struct AutoSaveSettings {
    bool autoEnabled = false;
    int intervalMinutes = 15;
    double jitter = 0.25;
    int maxPerHour = 3;
    int maxRetries = 3;
};

class AutoSaveManager {
public:
    static AutoSaveManager& instance();
    void init();
    void shutdown();
    bool triggerManualSave();
    void tick(float dt);
    std::vector<std::string> getLogs();
    AutoSaveSettings settings;

    // Exposed for tests
    bool simulateSaveButton();

private:
    AutoSaveManager();
    ~AutoSaveManager();
    bool invokeGameSaveByUISimulation();
    bool attemptInvokeWithRetries();
    void scheduleNext();
    int computeJitteredMinutes(int base);
    bool canAttemptNow();
    void recordAttempt(bool success);
    void log(const std::string& s);

    std::mutex m_mtx;
    std::atomic<bool> m_running{false};
    std::chrono::steady_clock::time_point m_nextTime;
    std::chrono::steady_clock::time_point m_hourStart;
    int m_attemptsThisHour = 0;
    std::vector<std::string> m_logs;
};
