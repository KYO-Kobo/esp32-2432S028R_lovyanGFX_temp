#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "MenuScreen.h"
#include "../ui/components/ModernButton.h"
#include "../shared/EventQueue.h"
#include <Arduino.h>

// グローバルイベントキュー（外部で定義）
extern EventQueue* g_touchEventQueue;

// スワイプ検出の閾値
#define SWIPE_THRESHOLD 50  // 最小スワイプ距離
#define SWIPE_TIME_LIMIT 500  // 最大スワイプ時間（ms）

MenuScreen::MenuScreen(LGFX* display) 
    : BaseScreen(display, SCREEN_MENU),
      touchStartX(0), touchStartY(0), touchStartTime(0), isTouching(false) {
    
    // ボタンを作成
    createButtons();
}

void MenuScreen::createButtons() {
    // ボタン1: デバイス設定（青いボタン、右下）
    auto deviceSettingsBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, 170, 170, 130, 40, "デバイス設定")
    );
    ButtonStyle blueStyle;
    blueStyle.normalColor = tft->color565(33, 150, 243);    // Material Blue
    blueStyle.pressedColor = tft->color565(25, 118, 210);   // Darker Blue
    blueStyle.cornerRadius = 10;
    blueStyle.shadowOffset = 4;
    deviceSettingsBtn->setStyle(blueStyle);
    deviceSettingsBtn->setOnClick([this]() {
        Serial.println("Device Settings button pressed");
        // 設定画面に遷移
        Event settingsEvent;
        settingsEvent.type = EVENT_SCREEN_CHANGE;
        settingsEvent.data.screenChange.targetScreen = SCREEN_SETTINGS;
        settingsEvent.data.screenChange.transition = TRANSITION_SLIDE_LEFT;
        
        if (g_touchEventQueue) {
            g_touchEventQueue->send(settingsEvent);
        }
    });
    buttons.push_back(std::move(deviceSettingsBtn));
    
    // ボタン5: 閉じる（右上、リセットボタンと同じスタイル）
    auto closeBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, tft->width() - 70, 10, 60, 30, "閉じる")
    );
    ButtonStyle closeStyle;
    closeStyle.normalColor = tft->color565(244, 67, 54);     // Material Red
    closeStyle.pressedColor = tft->color565(211, 47, 47);    // Darker Red
    closeStyle.cornerRadius = 0;  // 角なし
    closeStyle.shadowOffset = 3;
    closeStyle.borderWidth = 2;
    closeStyle.borderColor = tft->color565(183, 28, 28);     // Dark Red Border
    closeBtn->setStyle(closeStyle);
    closeBtn->setOnClick([this]() {
        Serial.println("Close button pressed - returning to Home");
        returnToHome();
    });
    buttons.push_back(std::move(closeBtn));
}

void MenuScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    
    // 日本語フォントを設定
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("メニュー");
    
    // 枠線を描画
    tft->drawRect(5, 50, tft->width() - 10, 2, TFT_DARKGREY);
    
    // デフォルトフォントに戻す
    tft->setFont(nullptr);
    tft->setTextSize(1);
    
    // ボタンを描画
    for (auto& button : buttons) {
        button->draw();
    }
}

void MenuScreen::draw() {
    // 必要に応じて画面全体を再描画
    if (needsRedraw) {
        init();
        needsRedraw = false;
    }
}

void MenuScreen::update() {
    // 状態更新（必要に応じて）
}

void MenuScreen::handleEvent(const Event& event) {
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

void MenuScreen::onEnter() {
    Serial.println("Entered Menu Screen");
    needsRedraw = true;
}

void MenuScreen::onExit() {
    Serial.println("Exiting Menu Screen");
}

void MenuScreen::onSwipeUp() {
    Serial.println("Menu: Swipe Up - return to Home");
    returnToHome();
}

void MenuScreen::onSwipeDown() {
    Serial.println("Menu: Swipe Down - return to Home");
    returnToHome();
}

void MenuScreen::onSwipeLeft() {
    Serial.println("Menu: Swipe Left - return to Home");
    returnToHome();
}

void MenuScreen::onSwipeRight() {
    Serial.println("Menu: Swipe Right - return to Home");
    returnToHome();
}

void MenuScreen::detectSwipeGesture(int32_t endX, int32_t endY) {
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

void MenuScreen::returnToHome() {
    // ホーム画面に戻るイベントを送信
    Event returnEvent;
    returnEvent.type = EVENT_SCREEN_CHANGE;
    returnEvent.data.screenChange.targetScreen = SCREEN_HOME;
    returnEvent.data.screenChange.transition = TRANSITION_NONE;
    
    // イベントキューに送信
    if (g_touchEventQueue) {
        g_touchEventQueue->send(returnEvent);
    }
}