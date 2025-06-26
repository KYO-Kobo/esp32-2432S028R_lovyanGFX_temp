#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "SettingsScreen.h"
#include "../ui/components/ModernButton.h"
#include "../shared/EventQueue.h"
#include <Arduino.h>

// グローバルイベントキュー（外部で定義）
extern EventQueue* g_touchEventQueue;

// スワイプ検出の閾値
#define SWIPE_THRESHOLD 50  // 最小スワイプ距離
#define SWIPE_TIME_LIMIT 500  // 最大スワイプ時間（ms）

SettingsScreen::SettingsScreen(LGFX* display) 
    : BaseScreen(display, SCREEN_SETTINGS),
      touchStartX(0), touchStartY(0), touchStartTime(0), isTouching(false),
      brightness(80), touchSound(false) {
    
    // ボタンを作成
    createButtons();
}

void SettingsScreen::createButtons() {
    // ボタン1: 明るさ調整（青いボタン）
    auto brightnessBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, 20, 100, 130, 40, "明るさ: 80%")
    );
    ButtonStyle blueStyle;
    blueStyle.normalColor = tft->color565(33, 150, 243);    // Material Blue
    blueStyle.pressedColor = tft->color565(25, 118, 210);   // Darker Blue
    blueStyle.cornerRadius = 10;
    blueStyle.shadowOffset = 4;
    brightnessBtn->setStyle(blueStyle);
    brightnessBtn->setOnClick([this]() {
        brightness = (brightness + 20) % 100;
        if (brightness == 0) brightness = 20;
        char buf[20];
        sprintf(buf, "明るさ: %d%%", brightness);
        buttons[0]->setText(buf);
        Serial.printf("Brightness changed to %d%%\n", brightness);
    });
    buttons.push_back(std::move(brightnessBtn));
    
    // ボタン2: タッチ音設定（緑のボタン）
    auto soundBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, 170, 100, 130, 40, "タッチ音: OFF")
    );
    ButtonStyle greenStyle;
    greenStyle.normalColor = tft->color565(76, 175, 80);    // Material Green
    greenStyle.pressedColor = tft->color565(56, 142, 60);   // Darker Green
    greenStyle.cornerRadius = 10;
    greenStyle.shadowOffset = 4;
    soundBtn->setStyle(greenStyle);
    soundBtn->setOnClick([this]() {
        touchSound = !touchSound;
        buttons[1]->setText(touchSound ? "タッチ音: ON" : "タッチ音: OFF");
        Serial.printf("Touch sound: %s\n", touchSound ? "ON" : "OFF");
    });
    buttons.push_back(std::move(soundBtn));
    
    // ボタン3: リセット（赤いボタン、角なし）
    auto resetBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, 20, 155, 130, 35, "リセット")
    );
    ButtonStyle redStyle;
    redStyle.normalColor = tft->color565(244, 67, 54);     // Material Red
    redStyle.pressedColor = tft->color565(211, 47, 47);    // Darker Red
    redStyle.cornerRadius = 0;  // 角なし
    redStyle.shadowOffset = 3;
    redStyle.borderWidth = 2;
    redStyle.borderColor = tft->color565(183, 28, 28);     // Dark Red Border
    resetBtn->setStyle(redStyle);
    resetBtn->setOnClick([this]() {
        brightness = 80;
        touchSound = false;
        buttons[0]->setText("明るさ: 80%");
        buttons[1]->setText("タッチ音: OFF");
        Serial.println("Settings reset to default");
    });
    buttons.push_back(std::move(resetBtn));
    
    // ボタン4: 保存（紫のボタン、ultra-thin）
    auto saveBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, 170, 155, 130, 35, "保存")
    );
    ButtonStyle purpleStyle;
    purpleStyle.normalColor = tft->color565(156, 39, 176);  // Material Purple
    purpleStyle.pressedColor = tft->color565(123, 31, 162); // Darker Purple
    purpleStyle.cornerRadius = 18;  // 完全に丸い角
    purpleStyle.shadowOffset = 2;   // 薄い影（ultra-thin）
    saveBtn->setStyle(purpleStyle);
    saveBtn->setOnClick([this]() {
        Serial.println("Settings saved!");
        // TODO: 実際の保存処理
    });
    buttons.push_back(std::move(saveBtn));
    
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
    
    // スワイプ案内（画面下部）
    tft->setTextColor(TFT_YELLOW);
    tft->setCursor(10, 205);
    tft->println("↑↓←→ スワイプでホームへ");
    
    // デフォルトフォントに戻す
    tft->setFont(nullptr);
    tft->setTextSize(1);
    
    // ボタンを描画
    for (auto& button : buttons) {
        button->draw();
    }
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