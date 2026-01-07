#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AutoSaveSettingsLayer : public CCNode {
public:
    static AutoSaveSettingsLayer* create();
    bool init() override;
private:
    void buildUI();
    void onSaveNow(CCObject* sender);
    void onViewLogs(CCObject* sender);
};

