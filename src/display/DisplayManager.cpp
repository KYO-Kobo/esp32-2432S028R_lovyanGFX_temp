#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "DisplayManager.h"
#include <Arduino.h>

DisplayManager::DisplayManager(LGFX* display) 
    : tft(display), dirty(false), needsRedraw(true),
      lastDisplayedX(-1), lastDisplayedY(-1), 
      lastDisplayedRawX(-1), lastDisplayedRawY(-1) {
    lastTouch = {EVENT_NONE, 0, 0, 0, 0, 0, 0};
}

void DisplayManager::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    
    // 日本語フォントを設定
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("デュアルコア タッチパネル");
    
    // 小さいフォントに戻す
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setCursor(10, 45);
    tft->println("コア0: 画面表示");
    tft->setCursor(10, 60);
    tft->println("コア1: タッチ処理");
    
    tft->setCursor(10, 80);
    tft->println("画面をタッチしてください...");
    
    // 画面情報表示
    tft->setCursor(10, 100);
    tft->printf("画面: %d x %d", tft->width(), tft->height());
    
    // 座標表示ラベル（英語フォントに戻す）
    tft->setFont(nullptr);  // デフォルトフォント
    tft->setTextSize(1);
    tft->setCursor(10, 120);
    tft->println("Touch Position:");
    tft->setCursor(10, 135);
    tft->print("X: ");
    tft->setCursor(10, 150);
    tft->print("Y: ");
    tft->setCursor(10, 170);
    tft->print("Raw: ");
}

void DisplayManager::update() {
    // イベントキューからタッチイベントを処理
    Event event;
    while (g_touchEventQueue && g_touchEventQueue->receive(event)) {
        handleEvent(event);
    }
    
    // 必要に応じて画面を更新
    if (needsRedraw) {
        redraw();
        needsRedraw = false;
    }
}

void DisplayManager::handleEvent(const Event& event) {
    switch (event.type) {
        case EVENT_TOUCH_DOWN:
        case EVENT_TOUCH_MOVE:
            updateTouchDisplay(
                event.data.touch.x,
                event.data.touch.y,
                event.data.touch.raw_x,
                event.data.touch.raw_y
            );
            // drawTouchPoint(event.data.touch.x, event.data.touch.y); // 赤い円を削除
            lastTouch = event.data.touch;
            break;
            
        case EVENT_TOUCH_UP:
            // タッチ終了時の処理
            break;
            
        default:
            break;
    }
}

void DisplayManager::redraw() {
    // 必要に応じて画面全体を再描画
}

void DisplayManager::updateTouchDisplay(int32_t x, int32_t y, int32_t raw_x, int32_t raw_y) {
    // 座標が変わった場合のみ更新
    if (x == lastDisplayedX && y == lastDisplayedY) {
        return;
    }
    
    // 前回の座標値をクリア（数値部分のみ、fillRectで確実にクリア）
    if (lastDisplayedX >= 0) {
        // X座標の数値部分をクリア（30～80の範囲）
        tft->fillRect(30, 135, 50, 12, TFT_BLACK);
        // Y座標の数値部分をクリア
        tft->fillRect(30, 150, 50, 12, TFT_BLACK);
    }
    
    // 新しい座標を表示
    tft->setTextColor(TFT_WHITE);
    tft->setTextSize(1);
    tft->setCursor(30, 135);
    tft->printf("%d", x);
    
    tft->setCursor(30, 150);
    tft->printf("%d", y);
    
    // Raw値が大きく変わった場合のみ更新
    if (abs(raw_x - lastDisplayedRawX) > 50 || abs(raw_y - lastDisplayedRawY) > 50) {
        // Raw値の表示部分をクリア（45～200の範囲）
        tft->fillRect(45, 170, 155, 12, TFT_BLACK);
        
        tft->setTextColor(TFT_YELLOW);
        tft->setTextSize(1);
        tft->setCursor(45, 170);
        tft->printf("X=%d, Y=%d", raw_x, raw_y);
        
        lastDisplayedRawX = raw_x;
        lastDisplayedRawY = raw_y;
    }
    
    lastDisplayedX = x;
    lastDisplayedY = y;
}

void DisplayManager::drawTouchPoint(int32_t x, int32_t y) {
    // タッチした位置に赤い円を描画
    tft->fillCircle(x, y, 3, TFT_RED);
}

void DisplayManager::updateStatus(const char* message) {
    tft->fillRect(10, 200, 300, 20, TFT_BLACK);
    tft->setTextColor(TFT_GREEN);
    tft->setCursor(10, 200);
    tft->print(message);
}