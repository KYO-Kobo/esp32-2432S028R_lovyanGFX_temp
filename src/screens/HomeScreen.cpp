#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "HomeScreen.h"
#include "../display/DisplayManager.h"
#include <Arduino.h>

// スワイプ検出の閾値
#define SWIPE_THRESHOLD 50  // 最小スワイプ距離
#define SWIPE_TIME_LIMIT 500  // 最大スワイプ時間（ms）

HomeScreen::HomeScreen(LGFX* display) 
    : BaseScreen(display, SCREEN_HOME),
      lastDisplayedX(-1), lastDisplayedY(-1),
      lastDisplayedRawX(-1), lastDisplayedRawY(-1),
      touchStartX(0), touchStartY(0), touchStartTime(0), isTouching(false) {
}

void HomeScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    
    // 日本語フォントを設定
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("ホーム画面");
    
    // 小さいフォントに戻す
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setCursor(10, 45);
    tft->println("コア0: 画面表示");
    tft->setCursor(10, 60);
    tft->println("コア1: タッチ処理");
    
    tft->setCursor(10, 80);
    tft->println("画面をタッチしてください...");
    
    // スワイプ案内
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, 95);
    tft->println("↑ 上にスワイプで設定画面へ");
    
    // 画面情報表示
    tft->setTextColor(TFT_WHITE);
    tft->setCursor(10, 115);
    tft->printf("画面: %d x %d", tft->width(), tft->height());
    
    // 座標表示ラベル（英語フォントに戻す）
    tft->setFont(nullptr);  // デフォルトフォント
    tft->setTextSize(1);
    tft->setCursor(10, 135);
    tft->println("Touch Position:");
    tft->setCursor(10, 150);
    tft->print("X: ");
    tft->setCursor(10, 165);
    tft->print("Y: ");
    tft->setCursor(10, 185);
    tft->print("Raw: ");
}

void HomeScreen::draw() {
    // 必要に応じて画面全体を再描画
    if (needsRedraw) {
        init();
        needsRedraw = false;
    }
}

void HomeScreen::update() {
    // 状態更新（必要に応じて）
}

void HomeScreen::handleEvent(const Event& event) {
    switch (event.type) {
        case EVENT_TOUCH_DOWN:
            // タッチ開始位置を記録
            touchStartX = event.data.touch.x;
            touchStartY = event.data.touch.y;
            touchStartTime = millis();
            isTouching = true;
            
            updateTouchDisplay(
                event.data.touch.x,
                event.data.touch.y,
                event.data.touch.raw_x,
                event.data.touch.raw_y
            );
            break;
            
        case EVENT_TOUCH_MOVE:
            updateTouchDisplay(
                event.data.touch.x,
                event.data.touch.y,
                event.data.touch.raw_x,
                event.data.touch.raw_y
            );
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
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
}

void HomeScreen::onEnter() {
    Serial.println("Entered Home Screen");
    needsRedraw = true;
}

void HomeScreen::onExit() {
    Serial.println("Exiting Home Screen");
}

void HomeScreen::onSwipeUp() {
    Serial.println("Home: Swipe Up detected - transition to Settings");
    // DisplayManagerがこのイベントを処理して画面遷移を行う
}

void HomeScreen::updateTouchDisplay(int32_t x, int32_t y, int32_t raw_x, int32_t raw_y) {
    // 座標が変わった場合のみ更新
    if (x == lastDisplayedX && y == lastDisplayedY) {
        return;
    }
    
    // 前回の座標値をクリア（数値部分のみ、fillRectで確実にクリア）
    if (lastDisplayedX >= 0) {
        // X座標の数値部分をクリア（30～80の範囲）
        tft->fillRect(30, 150, 50, 12, TFT_BLACK);
        // Y座標の数値部分をクリア
        tft->fillRect(30, 165, 50, 12, TFT_BLACK);
    }
    
    // 新しい座標を表示
    tft->setTextColor(TFT_WHITE);
    tft->setTextSize(1);
    tft->setCursor(30, 150);
    tft->printf("%d", x);
    
    tft->setCursor(30, 165);
    tft->printf("%d", y);
    
    // Raw値が大きく変わった場合のみ更新
    if (abs(raw_x - lastDisplayedRawX) > 50 || abs(raw_y - lastDisplayedRawY) > 50) {
        // Raw値の表示部分をクリア（45～200の範囲）
        tft->fillRect(45, 185, 155, 12, TFT_BLACK);
        
        tft->setTextColor(TFT_YELLOW);
        tft->setTextSize(1);
        tft->setCursor(45, 185);
        tft->printf("X=%d, Y=%d", raw_x, raw_y);
        
        lastDisplayedRawX = raw_x;
        lastDisplayedRawY = raw_y;
    }
    
    lastDisplayedX = x;
    lastDisplayedY = y;
}

void HomeScreen::detectSwipeGesture(int32_t endX, int32_t endY) {
    if (!isTouching) return;
    
    uint32_t swipeDuration = millis() - touchStartTime;
    if (swipeDuration > SWIPE_TIME_LIMIT) return;
    
    int32_t dx = endX - touchStartX;
    int32_t dy = endY - touchStartY;
    
    // 上方向のスワイプを検出（Y座標が減少）
    if (dy < -SWIPE_THRESHOLD && abs(dx) < abs(dy)) {
        onSwipeUp();
    }
}