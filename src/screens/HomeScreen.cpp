#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "HomeScreen.h"
#include "../display/DisplayManager.h"
#include <Arduino.h>

// 最小構成：ホーム画面は「ホーム画面」の文字のみ表示

HomeScreen::HomeScreen(LGFX* display) 
    : BaseScreen(display, SCREEN_HOME) {
}

void HomeScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("ホーム画面");
    tft->setFont(nullptr);
}

void HomeScreen::draw() {
    if (needsRedraw) { init(); needsRedraw = false; }
}

void HomeScreen::update() {
    // 何もしない（静的表示）
}

void HomeScreen::handleEvent(const Event& event) {
    (void)event; // 最小構成：イベントは無視
}

void HomeScreen::onEnter() {
    Serial.println("Entered Home Screen");
    needsRedraw = true;
}

void HomeScreen::onExit() {
    Serial.println("Exiting Home Screen");
}

void HomeScreen::onSwipeUp() {
    // 何もしない（ホーム固定）
}