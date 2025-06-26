#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "InfoScreen.h"
#include "../ui/components/ModernButton.h"
#include "../shared/EventQueue.h"
#include <Arduino.h>
#include <WiFi.h>

// グローバルイベントキュー（外部で定義）
extern EventQueue* g_touchEventQueue;

// スワイプ検出の閾値
#define SWIPE_THRESHOLD 50  // 最小スワイプ距離
#define SWIPE_TIME_LIMIT 500  // 最大スワイプ時間（ms）

// ビルド時に定義される情報
#ifndef APP_VERSION
#define APP_VERSION "1.0.0"
#endif

#ifndef BOARD_NAME
#define BOARD_NAME "ESP32-2432S028R"
#endif

#ifndef PRODUCT_NAME
#define PRODUCT_NAME "未定"
#endif

InfoScreen::InfoScreen(LGFX* display) 
    : BaseScreen(display, SCREEN_INFO),
      touchStartX(0), touchStartY(0), touchStartTime(0), isTouching(false) {
    
    // ボタンを作成
    createButtons();
    
    // システム情報を初期化
    updateSystemInfo();
}

void InfoScreen::createButtons() {
    // 戻るボタン（右上）
    auto backBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, tft->width() - 70, 10, 60, 30, "戻る")
    );
    ButtonStyle backStyle;
    backStyle.normalColor = tft->color565(96, 125, 139);    // Blue Grey
    backStyle.pressedColor = tft->color565(69, 90, 100);    // Darker Blue Grey
    backStyle.cornerRadius = 5;
    backStyle.shadowOffset = 2;
    backBtn->setStyle(backStyle);
    backBtn->setOnClick([this]() {
        Serial.println("Back button pressed - returning to Settings");
        returnToSettings();
    });
    buttons.push_back(std::move(backBtn));
}

void InfoScreen::updateSystemInfo() {
    // ボード名とプロダクト名
    boardName = BOARD_NAME;
    productName = PRODUCT_NAME;
    version = APP_VERSION;
    
    // ESP32のチップID（シリアル番号として使用）
    uint64_t chipid = ESP.getEfuseMac();
    chipId = String((uint32_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
    chipId.toUpperCase();
    
    // MACアドレス
    macAddress = WiFi.macAddress();
    
    // メモリ情報
    freeHeap = ESP.getFreeHeap();
    totalHeap = ESP.getHeapSize();
    
    // PSRAMメモリ情報
    freePsram = ESP.getFreePsram();
    totalPsram = ESP.getPsramSize();
    
    // フラッシュメモリ情報
    flashSize = ESP.getFlashChipSize();
}

void InfoScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    
    // タイトル
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("システム情報");
    
    // 枠線を描画
    tft->drawRect(5, 50, tft->width() - 10, 2, TFT_DARKGREY);
    
    // デフォルトフォントに戻す
    tft->setFont(nullptr);
    tft->setTextSize(1);
    
    // システム情報を表示
    int y = 70;
    const int lineHeight = 20;
    
    // ボード情報
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, y);
    tft->print("ボード: ");
    tft->setTextColor(TFT_WHITE);
    tft->setFont(nullptr);
    tft->println(boardName);
    y += lineHeight;
    
    // 製品名
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, y);
    tft->print("製品名: ");
    tft->setTextColor(TFT_WHITE);
    tft->setFont(nullptr);
    tft->println(productName);
    y += lineHeight;
    
    // バージョン
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, y);
    tft->print("バージョン: ");
    tft->setTextColor(TFT_WHITE);
    tft->setFont(nullptr);
    tft->println(version);
    y += lineHeight;
    
    // チップID
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, y);
    tft->print("チップID: ");
    tft->setTextColor(TFT_WHITE);
    tft->setFont(nullptr);
    tft->println(chipId);
    y += lineHeight;
    
    // MACアドレス
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, y);
    tft->print("MACアドレス: ");
    tft->setTextColor(TFT_WHITE);
    tft->setFont(nullptr);
    tft->println(macAddress);
    y += lineHeight;
    
    // フラッシュメモリ情報
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, y);
    tft->print("フラッシュ: ");
    tft->setTextColor(TFT_WHITE);
    tft->setFont(nullptr);
    tft->printf("%d MB", flashSize / 1024 / 1024);
    y += lineHeight;
    
    // RAM情報
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setTextColor(TFT_CYAN);
    tft->setCursor(10, y);
    tft->print("RAM: ");
    tft->setTextColor(TFT_WHITE);
    tft->setFont(nullptr);
    tft->printf("%d KB / %d KB", freeHeap / 1024, totalHeap / 1024);
    
    // PSRAM情報（もし存在すれば）
    if (totalPsram > 0) {
        y += lineHeight;
        tft->setFont(&fonts::lgfxJapanGothic_12);
        tft->setTextColor(TFT_CYAN);
        tft->setCursor(10, y);
        tft->print("PSRAM: ");
        tft->setTextColor(TFT_WHITE);
        tft->setFont(nullptr);
        tft->printf("%d KB / %d KB", freePsram / 1024, totalPsram / 1024);
    }
    
    // ボタンを描画
    for (auto& button : buttons) {
        button->draw();
    }
}

void InfoScreen::draw() {
    // 必要に応じて画面全体を再描画
    if (needsRedraw) {
        init();
        needsRedraw = false;
    }
}

void InfoScreen::update() {
    // 定期的にメモリ情報を更新
    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {  // 1秒ごと
        lastUpdate = millis();
        
        // RAM情報のみ更新
        uint32_t newFreeHeap = ESP.getFreeHeap();
        uint32_t newFreePsram = ESP.getFreePsram();
        
        if (newFreeHeap != freeHeap) {
            freeHeap = newFreeHeap;
            
            // RAM表示部分のみ更新
            int y = 170;  // RAM情報の表示位置
            tft->fillRect(50, y, 190, 16, TFT_BLACK);  // 前の表示をクリア
            tft->setCursor(50, y);
            tft->setTextColor(TFT_WHITE);
            tft->setFont(nullptr);
            tft->printf("%d KB / %d KB", freeHeap / 1024, totalHeap / 1024);
        }
        
        // PSRAM情報の更新（存在する場合）
        if (totalPsram > 0 && newFreePsram != freePsram) {
            freePsram = newFreePsram;
            
            int y = 190;  // PSRAM情報の表示位置
            tft->fillRect(60, y, 180, 16, TFT_BLACK);  // 前の表示をクリア
            tft->setCursor(60, y);
            tft->setTextColor(TFT_WHITE);
            tft->setFont(nullptr);
            tft->printf("%d KB / %d KB", freePsram / 1024, totalPsram / 1024);
        }
    }
}

void InfoScreen::handleEvent(const Event& event) {
    switch (event.type) {
        case EVENT_TOUCH_DOWN: {
            // タッチ開始位置を記録
            touchStartX = event.data.touch.x;
            touchStartY = event.data.touch.y;
            touchStartTime = millis();
            isTouching = true;
            
            // ボタンのタッチ処理
            for (auto& button : buttons) {
                button->handleTouch(event.data.touch.x, event.data.touch.y, true);
            }
            break;
        }
            
        case EVENT_TOUCH_MOVE: {
            // ボタンのタッチ移動処理
            for (auto& button : buttons) {
                button->handleTouch(event.data.touch.x, event.data.touch.y, true);
            }
            break;
        }
            
        case EVENT_TOUCH_UP: {
            // ボタンのタッチ終了処理
            bool buttonHandled = false;
            for (auto& button : buttons) {
                if (button->handleTouch(event.data.touch.x, event.data.touch.y, false)) {
                    buttonHandled = true;
                }
            }
            
            // ボタンが処理しなかった場合、スワイプジェスチャーを検出
            if (isTouching && !buttonHandled) {
                detectSwipeGesture(event.data.touch.x, event.data.touch.y);
            }
            isTouching = false;
            break;
        }
            
        case EVENT_GESTURE_SWIPE: {
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
        }
            
        default:
            break;
    }
}

void InfoScreen::onEnter() {
    Serial.println("Entered Info Screen");
    updateSystemInfo();  // 最新の情報を取得
    needsRedraw = true;
}

void InfoScreen::onExit() {
    Serial.println("Exiting Info Screen");
}

void InfoScreen::onSwipeUp() {
    Serial.println("Info: Swipe Up - return to Settings");
    returnToSettings();
}

void InfoScreen::onSwipeDown() {
    Serial.println("Info: Swipe Down - return to Settings");
    returnToSettings();
}

void InfoScreen::onSwipeLeft() {
    Serial.println("Info: Swipe Left - return to Settings");
    returnToSettings();
}

void InfoScreen::onSwipeRight() {
    Serial.println("Info: Swipe Right - return to Settings");
    returnToSettings();
}

void InfoScreen::detectSwipeGesture(int32_t endX, int32_t endY) {
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

void InfoScreen::returnToSettings() {
    // 設定画面に戻るイベントを送信
    Event returnEvent;
    returnEvent.type = EVENT_SCREEN_CHANGE;
    returnEvent.data.screenChange.targetScreen = SCREEN_SETTINGS;
    returnEvent.data.screenChange.transition = TRANSITION_SLIDE_RIGHT;
    
    // イベントキューに送信
    if (g_touchEventQueue) {
        g_touchEventQueue->send(returnEvent);
    }
}