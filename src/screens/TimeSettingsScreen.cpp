#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "TimeSettingsScreen.h"
#include <Arduino.h>

TimeSettingsScreen::TimeSettingsScreen(LGFX* display)
    : BaseScreen(display, SCREEN_TIME_SETTINGS) {}

void TimeSettingsScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("時間設定");
    tft->setFont(nullptr);
}

void TimeSettingsScreen::draw() {
    if (needsRedraw) { init(); needsRedraw = false; }
}

void TimeSettingsScreen::update() {
}

void TimeSettingsScreen::handleEvent(const Event& event) {
    (void)event;
}

void TimeSettingsScreen::onEnter() {
    needsRedraw = true;
}

void TimeSettingsScreen::onExit() {
}


