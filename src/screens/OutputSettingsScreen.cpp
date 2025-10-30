#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "OutputSettingsScreen.h"
#include <Arduino.h>

OutputSettingsScreen::OutputSettingsScreen(LGFX* display)
    : BaseScreen(display, SCREEN_OUTPUT_SETTINGS) {}

void OutputSettingsScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("出力設定");
    tft->setFont(nullptr);
}

void OutputSettingsScreen::draw() {
    if (needsRedraw) { init(); needsRedraw = false; }
}

void OutputSettingsScreen::update() {
}

void OutputSettingsScreen::handleEvent(const Event& event) {
    (void)event;
}

void OutputSettingsScreen::onEnter() {
    needsRedraw = true;
}

void OutputSettingsScreen::onExit() {
}


