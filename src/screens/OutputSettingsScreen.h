#ifndef OUTPUT_SETTINGS_SCREEN_H
#define OUTPUT_SETTINGS_SCREEN_H

#include "BaseScreen.h"

class OutputSettingsScreen : public BaseScreen {
public:
    OutputSettingsScreen(LGFX* display);
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    void onEnter() override;
    void onExit() override;
};

#endif // OUTPUT_SETTINGS_SCREEN_H
