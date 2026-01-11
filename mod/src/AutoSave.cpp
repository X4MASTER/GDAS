#include "AutoSave.hpp"
#include <ctime>
#include <random>
#include <thread>

AutoSaveManager& AutoSaveManager::instance() {
    static AutoSaveManager inst;
    return inst;
}

AutoSaveManager::AutoSaveManager() {
    settings.autoEnabled = false;
    settings.intervalMinutes = 15;
    settings.jitter = 0.25;
    settings.maxPerHour = 3;
    settings.maxRetries = 3;
    m_hourStart = std::chrono::steady_clock::now();
    scheduleNext();
}

AutoSaveManager::~AutoSaveManager() {
    shutdown();
}

void AutoSaveManager::init() {
    m_running = true;
    log("AutoSave initialized");
    scheduleNext();
    Scheduler::get()->schedule([this](float dt){ this->tick(dt); }, 0.5f, "autosave_tick");
}

void AutoSaveManager::shutdown() {
    m_running = false;
    Scheduler::get()->unschedule("autosave_tick");
    log("AutoSave shutdown");
}

void AutoSaveManager::tick(float) {
    if (!m_running) return;
    if (!settings.autoEnabled) return;
    auto now = std::chrono::steady_clock::now();
    if (now - m_hourStart >= std::chrono::hours(1)) {
        m_hourStart = now;
        m_attemptsThisHour = 0;
    }
    if (now >= m_nextTime && canAttemptNow()) {
        bool ok = attemptInvokeWithRetries();
        recordAttempt(ok);
        if (!ok) {
            log("AutoSave suspended for 30 minutes due to repeated failures");
            m_nextTime = std::chrono::steady_clock::now() + std::chrono::minutes(30);
        } else {
            log("AutoSave: save invoked successfully");
            scheduleNext();
        }
    }
}

bool AutoSaveManager::triggerManualSave() {
    if (!m_running) return false;
    std::lock_guard<std::mutex> lk(m_mtx);
    bool ok = attemptInvokeWithRetries();
    if (ok) log("Manual save invoked successfully");
    else log("Manual save invocation failed");
    return ok;
}

bool AutoSaveManager::attemptInvokeWithRetries() {
    int attempt = 0;
    int baseDelay = 200; // ms
    while (attempt <= settings.maxRetries) {
        bool ok = invokeGameSaveByUISimulation();
        if (ok) return true;
        attempt++;
        std::this_thread::sleep_for(std::chrono::milliseconds(baseDelay * (1 << attempt)));
    }
    return false;
}

static CCNode* find_save_node_recursive(CCNode* node, int depth = 0) {
    if (!node || depth > 10) return nullptr;
    const char* name = node->getName();
    if (name) {
        if (strcasestr(name, "save")) return node;
    }
    auto children = node->getChildren();
    if (!children) return nullptr;
    CCObject* obj = nullptr;
    CCARRAY_FOREACH(children, obj) {
        CCNode* child = static_cast<CCNode*>(obj);
        CCNode* res = find_save_node_recursive(child, depth + 1);
        if (res) return res;
    }
    return nullptr;
}

bool AutoSaveManager::invokeGameSaveByUISimulation() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return false;
    CCNode* found = find_save_node_recursive(scene);
    if (!found) {
        // try common menu layers
        auto children = scene->getChildren();
        if (children) {
            CCObject* obj = nullptr;
            CCARRAY_FOREACH(children, obj) {
                CCNode* child = static_cast<CCNode*>(obj);
                CCNode* res = find_save_node_recursive(child, 1);
                if (res) { found = res; break; }
            }
        }
    }
    if (!found) return false;
    if (auto menuItem = dynamic_cast<CCMenuItem*>(found)) {
        menuItem->activate();
        return true;
    }
    if (auto control = dynamic_cast<CCControlButton*>(found)) {
        control->sendActionsForControlEvents(kCCControlEventTouchUpInside);
        return true;
    }
    try { found->performSelector(SEL_CallFuncO, found); } catch(...) {}
    return true;
}

bool AutoSaveManager::simulateSaveButton() {
    std::lock_guard<std::mutex> lk(m_mtx);
    return invokeGameSaveByUISimulation();
}

void AutoSaveManager::scheduleNext() {
    int base = settings.intervalMinutes;
    if (base < 10) base = 10;
    int minutes = computeJitteredMinutes(base);
    m_nextTime = std::chrono::steady_clock::now() + std::chrono::minutes(minutes);
    log("Next scheduled save in " + std::to_string(minutes) + " minutes");
}

int AutoSaveManager::computeJitteredMinutes(int base) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    double p = settings.jitter;
    std::uniform_real_distribution<> dis(1.0 - p, 1.0 + p);
    double factor = dis(gen);
    int val = static_cast<int>(std::max(1.0, std::round(base * factor)));
    return val;
}

bool AutoSaveManager::canAttemptNow() {
    return m_attemptsThisHour < settings.maxPerHour;
}

void AutoSaveManager::recordAttempt(bool success) {
    m_attemptsThisHour++;
    (void)success;
}

void AutoSaveManager::log(const std::string& s) {
    auto t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::gmtime(&t));
    m_logs.push_back(std::string(buf) + " - " + s);
    if (m_logs.size() > 200) m_logs.erase(m_logs.begin());
    log::info("AutoSave: {}", s);
}

std::vector<std::string> AutoSaveManager::getLogs() {
    return m_logs;
}
