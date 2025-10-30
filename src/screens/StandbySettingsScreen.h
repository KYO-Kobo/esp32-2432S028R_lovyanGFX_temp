#ifndef STANDBY_SETTINGS_SCREEN_H
#define STANDBY_SETTINGS_SCREEN_H

#include "BaseScreen.h"

class StandbySettingsScreen : public BaseScreen {
public:
    StandbySettingsScreen(LGFX* display);
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    void onEnter() override;
    void onExit() override;
};

#endif // STANDBY_SETTINGS_SCREEN_H
