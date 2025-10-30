#ifndef LOG_SCREEN_H
#define LOG_SCREEN_H

#include "BaseScreen.h"

class LogScreen : public BaseScreen {
public:
    LogScreen(LGFX* display);
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    void onEnter() override;
    void onExit() override;
};

#endif // LOG_SCREEN_H
