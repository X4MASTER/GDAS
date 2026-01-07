#include "SettingsLayer.hpp"
#include "AutoSave.hpp"

AutoSaveSettingsLayer* AutoSaveSettingsLayer::create() {
    auto ret = new AutoSaveSettingsLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool AutoSaveSettingsLayer::init() {
    if (!CCNode::init()) return false;
    buildUI();
    return true;
}

void AutoSaveSettingsLayer::buildUI() {
    auto win = CCDirector::sharedDirector()->getWinSize();
    auto title = CCLabelBMFont::create("Auto Save", "bigFont.fnt");
    title->setPosition({win.width/2, win.height - 60});
    addChild(title);
    auto saveNowBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_button_01.png"),
        this, menu_selector(AutoSaveSettingsLayer::onSaveNow)
    );
    auto lbl = CCLabelBMFont::create("Save Now", "goldFont.fnt");
    lbl->setPosition({saveNowBtn->getContentSize().width/2, saveNowBtn->getContentSize().height/2});
    saveNowBtn->addChild(lbl);
    saveNowBtn->setPosition({win.width/2, win.height - 140});
    auto logsBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_button_01.png"),
        this, menu_selector(AutoSaveSettingsLayer::onViewLogs)
    );
    auto lbl2 = CCLabelBMFont::create("View Logs", "goldFont.fnt");
    lbl2->setPosition({logsBtn->getContentSize().width/2, logsBtn->getContentSize().height/2});
    logsBtn->addChild(lbl2);
    logsBtn->setPosition({win.width/2, win.height - 200});
    auto menu = CCMenu::create(saveNowBtn, logsBtn, nullptr);
    menu->setPosition({0,0});
    addChild(menu);
}

void AutoSaveSettingsLayer::onSaveNow(CCObject* sender) {
    FLAlertLayer::create("Auto Save", "Trigger the game's save now? This will attempt to simulate the game's save button.", "Yes", "No",
        [](auto, auto){ bool ok = AutoSaveManager::instance().triggerManualSave(); if (ok) FLAlertLayer::create("Auto Save","Save invoked","OK")->show(); else FLAlertLayer::create("Auto Save","Could not find save control","OK")->show(); },
        nullptr)->show();
}

void AutoSaveSettingsLayer::onViewLogs(CCObject* sender) {
    auto logs = AutoSaveManager::instance().getLogs();
    std::string text;
    for (auto& l : logs) { text += l + "\n"; }
    if (text.empty()) text = "No logs";
    FLAlertLayer::create("Auto Save Logs", text.c_str(), "OK")->show();
}
