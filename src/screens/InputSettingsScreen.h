#ifndef INPUT_SETTINGS_SCREEN_H
#define INPUT_SETTINGS_SCREEN_H

#include "BaseScreen.h"

class InputSettingsScreen : public BaseScreen {
public:
    InputSettingsScreen(LGFX* display);
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    void onEnter() override;
    void onExit() override;
};

#endif // INPUT_SETTINGS_SCREEN_H
