#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "LogScreen.h"
#include <Arduino.h>
#include "../ui/components/ModernButton.h"
#include "../shared/EventQueue.h"
#include <memory>
#include <vector>

extern EventQueue* g_touchEventQueue;

LogScreen::LogScreen(LGFX* display)
    : BaseScreen(display, SCREEN_LOG) {}

void LogScreen::createButtons() {
    buttons.clear();
    auto backBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, tft->width() - 70, 10, 60, 30, "戻る")
    );
    ButtonStyle backStyle;
    backStyle.normalColor = tft->color565(96, 125, 139);    // Blue Grey
    backStyle.pressedColor = tft->color565(69, 90, 100);
    backStyle.cornerRadius = 5;
    backStyle.shadowOffset = 2;
    backBtn->setStyle(backStyle);
    backBtn->setOnClick([this]() {
        Event e;
        e.type = EVENT_SCREEN_CHANGE;
        e.data.screenChange.targetScreen = SCREEN_MENU;
        e.data.screenChange.transition = TRANSITION_SLIDE_RIGHT;
        if (g_touchEventQueue) g_touchEventQueue->send(e);
    });
    buttons.push_back(std::move(backBtn));
}

void LogScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("ログ");
    tft->setFont(nullptr);
    for (auto& button : buttons) {
        button->draw();
    }
}

void LogScreen::draw() {
    if (needsRedraw) { init(); needsRedraw = false; }
}

void LogScreen::update() {
}

void LogScreen::handleEvent(const Event& event) {
    switch (event.type) {
        case EVENT_TOUCH_DOWN:
        case EVENT_TOUCH_MOVE:
            for (auto& button : buttons) {
                button->handleTouch(event.data.touch.x, event.data.touch.y, true);
            }
            break;
        case EVENT_TOUCH_UP: {
            for (auto& button : buttons) {
                button->handleTouch(event.data.touch.x, event.data.touch.y, false);
            }
            break;
        }
        default:
            break;
    }
}

void LogScreen::onEnter() {
    createButtons();
    needsRedraw = true;
}

void LogScreen::onExit() {
}


