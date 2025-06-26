#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "SettingsScreen.h"
#include <Arduino.h>

// スワイプ検出の閾値
#define SWIPE_THRESHOLD 50  // 最小スワイプ距離
#define SWIPE_TIME_LIMIT 500  // 最大スワイプ時間（ms）

SettingsScreen::SettingsScreen(LGFX* display) 
    : BaseScreen(display, SCREEN_SETTINGS),
      touchStartX(0), touchStartY(0), touchStartTime(0), isTouching(false),
      brightness(80), touchSound(false) {
}

void SettingsScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    
    // 日本語フォントを設定
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("設定画面");
    
    // 戻る方法の説明
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, 45);
    tft->println("どの方向にスワイプしても");
    tft->setCursor(10, 60);
    tft->println("ホーム画面に戻ります");
    
    // 枠線を描画
    tft->drawRect(5, 75, tft->width() - 10, 2, TFT_DARKGREY);
    
    // 設定項目
    tft->setTextColor(TFT_WHITE);
    tft->setCursor(10, 85);
    tft->println("設定項目:");
    
    // 明度設定
    tft->setCursor(20, 105);
    tft->printf("画面の明るさ: %d%%", brightness);
    
    // タッチ音設定
    tft->setCursor(20, 125);
    tft->printf("タッチ音: %s", touchSound ? "ON" : "OFF");
    
    // スワイプ案内（画面下部）
    tft->setTextColor(TFT_YELLOW);
    tft->setCursor(10, 160);
    tft->println("↑↓←→ スワイプでホームへ");
    
    // デフォルトフォントに戻す
    tft->setFont(nullptr);
    tft->setTextSize(1);
}

void SettingsScreen::draw() {
    // 必要に応じて画面全体を再描画
    if (needsRedraw) {
        init();
        needsRedraw = false;
    }
}

void SettingsScreen::update() {
    // 状態更新（必要に応じて）
}

void SettingsScreen::handleEvent(const Event& event) {
    switch (event.type) {
        case EVENT_TOUCH_DOWN:
            // タッチ開始位置を記録
            touchStartX = event.data.touch.x;
            touchStartY = event.data.touch.y;
            touchStartTime = millis();
            isTouching = true;
            break;
            
        case EVENT_TOUCH_UP:
            if (isTouching) {
                // スワイプジェスチャーを検出
                detectSwipeGesture(event.data.touch.x, event.data.touch.y);
                isTouching = false;
            }
            break;
            
        case EVENT_GESTURE_SWIPE:
            // TouchManagerからのジェスチャーイベント
            switch (event.data.gesture.direction) {
                case GestureEvent::GESTURE_UP:
                    onSwipeUp();
                    break;
                case GestureEvent::GESTURE_DOWN:
                    onSwipeDown();
                    break;
                case GestureEvent::GESTURE_LEFT:
                    onSwipeLeft();
                    break;
                case GestureEvent::GESTURE_RIGHT:
                    onSwipeRight();
                    break;
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
}

void SettingsScreen::onEnter() {
    Serial.println("Entered Settings Screen");
    needsRedraw = true;
}

void SettingsScreen::onExit() {
    Serial.println("Exiting Settings Screen");
}

void SettingsScreen::onSwipeUp() {
    Serial.println("Settings: Swipe Up - return to Home");
    returnToHome();
}

void SettingsScreen::onSwipeDown() {
    Serial.println("Settings: Swipe Down - return to Home");
    returnToHome();
}

void SettingsScreen::onSwipeLeft() {
    Serial.println("Settings: Swipe Left - return to Home");
    returnToHome();
}

void SettingsScreen::onSwipeRight() {
    Serial.println("Settings: Swipe Right - return to Home");
    returnToHome();
}

void SettingsScreen::detectSwipeGesture(int32_t endX, int32_t endY) {
    if (!isTouching) return;
    
    uint32_t swipeDuration = millis() - touchStartTime;
    if (swipeDuration > SWIPE_TIME_LIMIT) return;
    
    int32_t dx = endX - touchStartX;
    int32_t dy = endY - touchStartY;
    
    // 各方向のスワイプを検出
    if (abs(dy) > abs(dx)) {
        // 縦方向のスワイプ
        if (dy < -SWIPE_THRESHOLD) {
            onSwipeUp();
        } else if (dy > SWIPE_THRESHOLD) {
            onSwipeDown();
        }
    } else {
        // 横方向のスワイプ
        if (dx < -SWIPE_THRESHOLD) {
            onSwipeLeft();
        } else if (dx > SWIPE_THRESHOLD) {
            onSwipeRight();
        }
    }
}

void SettingsScreen::returnToHome() {
    // DisplayManagerがこのイベントを処理して画面遷移を行う
}