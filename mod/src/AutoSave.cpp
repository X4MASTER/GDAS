
#include "AutoSave.hpp"
#include <Geode/Geode.hpp>
#include <ctime>
#include <functional>
using namespace geode::prelude;

AutoSaveManager& AutoSaveManager::instance() {
    static AutoSaveManager inst;
    return inst;
}

AutoSaveManager::AutoSaveManager() {
    settings.enableManualTrigger = true;
}

AutoSaveManager::~AutoSaveManager() {
    shutdown();
}

void AutoSaveManager::init() {
    m_running = true;
    log("AutoSave initialized");
}

void AutoSaveManager::shutdown() {
    m_running = false;
    log("AutoSave shutdown");
}

bool AutoSaveManager::triggerManualSave() {
    if (!m_running) return false;
    std::lock_guard<std::mutex> lk(m_mtx);
    bool ok = invokeGameSaveByUISimulation();
    if (ok) log("Manual save invoked successfully");
    else log("Manual save invocation failed");
    return ok;
}

bool AutoSaveManager::invokeGameSaveByUISimulation() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return false;
    std::function<CCNode*(CCNode*)> findNode = [&](CCNode* node)->CCNode* {
        if (!node) return nullptr;
        const char* name = node->getName();
        if (name) {
            if (strstr(name, "save") || strstr(name, "Save") || strstr(name, "SAVE")) return node;
        }
        auto children = node->getChildren();
        if (children) {
            CCObject* obj = nullptr;
            CCARRAY_FOREACH(children, obj) {
                CCNode* child = static_cast<CCNode*>(obj);
                CCNode* res = findNode(child);
                if (res) return res;
            }
        }
        return nullptr;
    };
    CCNode* root = scene;
    CCNode* found = findNode(root);
    if (!found) return false;
    auto menuItem = dynamic_cast<CCMenuItem*>(found);
    if (menuItem) {
        menuItem->activate();
        return true;
    }
    // best-effort: try to call a selector if available
    try {
        found->performSelector(SEL_CallFuncO, found);
    } catch (...) {}
    return true;
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
