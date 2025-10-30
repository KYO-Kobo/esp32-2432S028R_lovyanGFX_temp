#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "LogScreen.h"
#include <Arduino.h>

LogScreen::LogScreen(LGFX* display)
    : BaseScreen(display, SCREEN_LOG) {}

void LogScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("ログ");
    tft->setFont(nullptr);
}

void LogScreen::draw() {
    if (needsRedraw) { init(); needsRedraw = false; }
}

void LogScreen::update() {
}

void LogScreen::handleEvent(const Event& event) {
    (void)event;
}

void LogScreen::onEnter() {
    needsRedraw = true;
}

void LogScreen::onExit() {
}


