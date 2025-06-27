#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "SettingsScreen.h"
#include "../ui/components/ModernButton.h"
#include "../ui/components/ConfirmDialog.h"
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
      brightness(80), showingDialog(false) {
    
    // ボタンを作成
    createButtons();
    
    // 初期の明るさを設定（80%）
    tft->setBrightness(brightness * 255 / 100);
}

void SettingsScreen::createButtons() {
    // ボタン1: 明るさ調整（青いボタン）
    char brightnessText[20];
    sprintf(brightnessText, "明るさ: %d%%", brightness);
    auto brightnessBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, 20, 70, 130, 40, brightnessText)
    );
    ButtonStyle blueStyle;
    blueStyle.normalColor = tft->color565(33, 150, 243);    // Material Blue
    blueStyle.pressedColor = tft->color565(25, 118, 210);   // Darker Blue
    blueStyle.cornerRadius = 10;
    blueStyle.shadowOffset = 4;
    brightnessBtn->setStyle(blueStyle);
    brightnessBtn->setOnClick([this]() {
        // 次の値に更新: 80% → 100% → 20% → 40% → 60% → 80%
        brightness = brightness + 20;
        if (brightness > 100) {
            brightness = 20;
        }
        
        // 新しい値を表示と実際の明るさに適用
        char buf[20];
        sprintf(buf, "明るさ: %d%%", brightness);
        buttons[0]->setText(buf);
        buttons[0]->draw();  // 即座に再描画
        tft->setBrightness(brightness * 255 / 100);
        
        Serial.printf("Brightness changed to %d%%\n", brightness);
    });
    buttons.push_back(std::move(brightnessBtn));
    
    // ボタン2: 情報（緑のボタン）
    auto infoBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, 170, 70, 130, 40, "情報")
    );
    ButtonStyle greenStyle;
    greenStyle.normalColor = tft->color565(76, 175, 80);    // Material Green
    greenStyle.pressedColor = tft->color565(56, 142, 60);   // Darker Green
    greenStyle.cornerRadius = 10;
    greenStyle.shadowOffset = 4;
    infoBtn->setStyle(greenStyle);
    infoBtn->setOnClick([this]() {
        Serial.println("Information button pressed");
        // 情報画面への遷移
        Event infoEvent;
        infoEvent.type = EVENT_SCREEN_CHANGE;
        infoEvent.data.screenChange.targetScreen = SCREEN_INFO;
        infoEvent.data.screenChange.transition = TRANSITION_SLIDE_LEFT;
        
        if (g_touchEventQueue) {
            g_touchEventQueue->send(infoEvent);
        }
    });
    buttons.push_back(std::move(infoBtn));
    
    
    // ボタン3: リセット（赤いボタン）
    auto resetBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, 95, 180, 130, 35, "リセット")
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
        Serial.println("Reset button pressed - showing confirmation");
        
        // 確認ダイアログを作成
        confirmDialog.reset(new ConfirmDialog(
            tft,
            "確認",
            "本当にリセットしてもいいですか？"
        ));
        
        // はいボタンのコールバック
        confirmDialog->setOnYes([this]() {
            Serial.println("Reset confirmed - going to home");
            showingDialog = false;
            
            // 明るさを初期値（80%）に戻す
            brightness = 80;
            tft->setBrightness(brightness * 255 / 100);
            
            // ホーム画面に戻る
            Event homeEvent;
            homeEvent.type = EVENT_SCREEN_CHANGE;
            homeEvent.data.screenChange.targetScreen = SCREEN_HOME;
            homeEvent.data.screenChange.transition = TRANSITION_FADE;
            
            if (g_touchEventQueue) {
                g_touchEventQueue->send(homeEvent);
                
                // リセットメッセージ表示イベントを送信
                Event resetMsgEvent;
                resetMsgEvent.type = EVENT_SHOW_RESET_MESSAGE;
                g_touchEventQueue->send(resetMsgEvent);
            }
        });
        
        // いいえボタンのコールバック
        confirmDialog->setOnNo([this]() {
            Serial.println("Reset cancelled");
            showingDialog = false;
            needsRedraw = true;  // 画面を再描画
        });
        
        // ダイアログを表示
        showingDialog = true;
        confirmDialog->show();
    });
    buttons.push_back(std::move(resetBtn));
    
    // ボタン4: 戻る（右上）
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
        Serial.println("Back button pressed - returning to Menu");
        returnToMenu();
    });
    buttons.push_back(std::move(backBtn));
}

void SettingsScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    
    // 日本語フォントを設定
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("デバイス設定");
    
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
    // ダイアログ表示中は、ダイアログのみがタッチイベントを処理
    if (showingDialog && confirmDialog) {
        switch (event.type) {
            case EVENT_TOUCH_DOWN:
            case EVENT_TOUCH_MOVE:
                confirmDialog->handleTouch(event.data.touch.x, event.data.touch.y, true);
                return;
                
            case EVENT_TOUCH_UP:
                confirmDialog->handleTouch(event.data.touch.x, event.data.touch.y, false);
                return;
                
            default:
                return;
        }
    }
    
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
    Serial.printf("Current brightness: %d%%\n", brightness);
    needsRedraw = true;
    
    // 現在の明るさを適用
    tft->setBrightness(brightness * 255 / 100);
    
    // 明るさボタンの表示を現在の値に同期
    if (buttons.size() > 0) {
        char buf[20];
        sprintf(buf, "明るさ: %d%%", brightness);
        buttons[0]->setText(buf);
        buttons[0]->redraw();  // 強制的に再描画
        Serial.printf("Updated button text to: %s\n", buf);
    }
}

void SettingsScreen::onExit() {
    Serial.println("Exiting Settings Screen");
}

void SettingsScreen::onSwipeUp() {
    Serial.println("Settings: Swipe Up - return to Menu");
    returnToMenu();
}

void SettingsScreen::onSwipeDown() {
    Serial.println("Settings: Swipe Down - return to Menu");
    returnToMenu();
}

void SettingsScreen::onSwipeLeft() {
    Serial.println("Settings: Swipe Left - return to Menu");
    returnToMenu();
}

void SettingsScreen::onSwipeRight() {
    Serial.println("Settings: Swipe Right - return to Menu");
    returnToMenu();
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

void SettingsScreen::returnToMenu() {
    // メニュー画面に戻るイベントを送信
    Event returnEvent;
    returnEvent.type = EVENT_SCREEN_CHANGE;
    returnEvent.data.screenChange.targetScreen = SCREEN_MENU;
    returnEvent.data.screenChange.transition = TRANSITION_SLIDE_RIGHT;
    
    // イベントキューに送信
    if (g_touchEventQueue) {
        g_touchEventQueue->send(returnEvent);
    }
}