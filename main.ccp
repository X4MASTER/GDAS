#include <Geode/Geode.hpp>
#include "AutoSave.hpp"
#include "SettingsLayer.hpp"
using namespace geode::prelude;

class AutoSaveMod : public Mod {
public:
    std::string getName() override { return "Auto Save"; }
    std::string getVersion() override { return "1.0.0"; }
    void onLoad() override {
        AutoSaveManager::instance().init();
        // register settings layer if available in your menu system
    }
    void onUnload() override {
        AutoSaveManager::instance().shutdown();
    }
};

CREATE_GEODE_PLUGIN(AutoSaveMod);
