#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "StandbySettingsScreen.h"
#include <Arduino.h>

StandbySettingsScreen::StandbySettingsScreen(LGFX* display)
    : BaseScreen(display, SCREEN_STANDBY_SETTINGS) {}

void StandbySettingsScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("待機設定");
    tft->setFont(nullptr);
}

void StandbySettingsScreen::draw() {
    if (needsRedraw) { init(); needsRedraw = false; }
}

void StandbySettingsScreen::update() {
}

void StandbySettingsScreen::handleEvent(const Event& event) {
    (void)event;
}

void StandbySettingsScreen::onEnter() {
    needsRedraw = true;
}

void StandbySettingsScreen::onExit() {
}


