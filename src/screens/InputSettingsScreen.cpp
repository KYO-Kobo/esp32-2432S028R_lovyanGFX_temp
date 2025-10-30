#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "InputSettingsScreen.h"
#include <Arduino.h>

InputSettingsScreen::InputSettingsScreen(LGFX* display)
    : BaseScreen(display, SCREEN_INPUT_SETTINGS) {}

void InputSettingsScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("入力設定");
    tft->setFont(nullptr);
}

void InputSettingsScreen::draw() {
    if (needsRedraw) { init(); needsRedraw = false; }
}

void InputSettingsScreen::update() {
}

void InputSettingsScreen::handleEvent(const Event& event) {
    (void)event;
}

void InputSettingsScreen::onEnter() {
    needsRedraw = true;
}

void InputSettingsScreen::onExit() {
}


