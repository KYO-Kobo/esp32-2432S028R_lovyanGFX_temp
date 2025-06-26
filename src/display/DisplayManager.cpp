#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "DisplayManager.h"
#include "../screens/ScreenManager.h"
#include "../screens/BaseScreen.h"
#include <Arduino.h>

DisplayManager::DisplayManager(LGFX* display) 
    : tft(display), dirty(false), needsRedraw(true) {
    lastTouch = {EVENT_NONE, 0, 0, 0, 0, 0, 0};
}

DisplayManager::~DisplayManager() {
    // unique_ptrが自動的にScreenManagerを削除
}

void DisplayManager::init() {
    // 画面管理を初期化
    screenManager.reset(new ScreenManager(tft));
    screenManager->init();
    
    Serial.println("DisplayManager initialized with ScreenManager");
}

void DisplayManager::update() {
    // イベントキューからタッチイベントを処理
    Event event;
    while (g_touchEventQueue && g_touchEventQueue->receive(event)) {
        handleEvent(event);
    }
    
    // 画面の更新
    if (screenManager) {
        screenManager->update();
    }
}

void DisplayManager::handleEvent(const Event& event) {
    // 画面管理にイベントを渡す
    if (screenManager) {
        screenManager->handleEvent(event);
        
        // スワイプジェスチャーの特別処理
        if (event.type == EVENT_GESTURE_SWIPE) {
            onSwipeDetected(event.data.gesture.direction);
        }
    }
}

void DisplayManager::redraw() {
    // 現在の画面を再描画
    if (screenManager && screenManager->getCurrentScreen()) {
        screenManager->getCurrentScreen()->draw();
    }
}

void DisplayManager::updateTouchDisplay(int32_t x, int32_t y, int32_t raw_x, int32_t raw_y) {
    // この機能は各画面クラスに移動
}

void DisplayManager::drawTouchPoint(int32_t x, int32_t y) {
    // タッチポイントの描画（現在は使用していない）
}

void DisplayManager::updateStatus(const char* message) {
    tft->fillRect(10, 200, 300, 20, TFT_BLACK);
    tft->setTextColor(TFT_GREEN);
    tft->setCursor(10, 200);
    tft->print(message);
}

void DisplayManager::onSwipeDetected(int direction) {
    if (!screenManager) return;
    
    ScreenID currentScreen = screenManager->getCurrentScreenId();
    
    // ホーム画面で上スワイプ → メニュー画面
    if (currentScreen == SCREEN_HOME && direction == GestureEvent::GESTURE_UP) {
        screenManager->transitionTo(SCREEN_MENU, TRANSITION_SLIDE_UP);
    }
    // メニュー画面で任意方向のスワイプ → ホーム画面
    else if (currentScreen == SCREEN_MENU) {
        screenManager->transitionTo(SCREEN_HOME, TRANSITION_SLIDE_DOWN);
    }
}