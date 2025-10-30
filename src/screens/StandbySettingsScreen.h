#ifndef STANDBY_SETTINGS_SCREEN_H
#define STANDBY_SETTINGS_SCREEN_H

#include "BaseScreen.h"
#include <memory>
#include <vector>
// 前方宣言
class ModernButton;

class StandbySettingsScreen : public BaseScreen {
private:
    std::vector<std::unique_ptr<ModernButton>> buttons;
public:
    StandbySettingsScreen(LGFX* display);
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    void onEnter() override;
    void onExit() override;
private:
    void createButtons();
};

#endif // STANDBY_SETTINGS_SCREEN_H
