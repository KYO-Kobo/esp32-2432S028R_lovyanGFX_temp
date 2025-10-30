#ifndef TIME_SETTINGS_SCREEN_H
#define TIME_SETTINGS_SCREEN_H

#include "BaseScreen.h"

class TimeSettingsScreen : public BaseScreen {
public:
    TimeSettingsScreen(LGFX* display);
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    void onEnter() override;
    void onExit() override;
};

#endif // TIME_SETTINGS_SCREEN_H
