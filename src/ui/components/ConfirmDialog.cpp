#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "ConfirmDialog.h"
#include <Arduino.h>

ConfirmDialog::ConfirmDialog(LGFX* display, const String& title, const String& message) 
    : tft(display), title(title), message(message) {
    
    // ダイアログのサイズと位置を計算（画面中央）
    width = 240;
    height = 140;
    x = (tft->width() - width) / 2;
    y = (tft->height() - height) / 2;
    
    createButtons();
}

void ConfirmDialog::createButtons() {
    // はいボタン（左側）
    yesButton.reset(new ModernButton(
        tft, 
        x + 20,                    // 左側
        y + height - 50,           // 下部
        90,                        // 幅
        35,                        // 高さ
        "はい"
    ));
    
    ButtonStyle yesStyle;
    yesStyle.normalColor = tft->color565(76, 175, 80);     // Material Green
    yesStyle.pressedColor = tft->color565(56, 142, 60);    // Darker Green
    yesStyle.cornerRadius = 8;
    yesStyle.shadowOffset = 0;  // 影を無くす
    yesButton->setStyle(yesStyle);
    yesButton->setOnClick([this]() {
        if (onYesCallback) {
            onYesCallback();
        }
    });
    
    // いいえボタン（右側）
    noButton.reset(new ModernButton(
        tft, 
        x + width - 110,           // 右側
        y + height - 50,           // 下部
        90,                        // 幅
        35,                        // 高さ
        "いいえ"
    ));
    
    ButtonStyle noStyle;
    noStyle.normalColor = tft->color565(158, 158, 158);    // Material Grey
    noStyle.pressedColor = tft->color565(97, 97, 97);      // Darker Grey
    noStyle.cornerRadius = 8;
    noStyle.shadowOffset = 0;  // 影を無くす
    noButton->setStyle(noStyle);
    noButton->setOnClick([this]() {
        if (onNoCallback) {
            onNoCallback();
        }
    });
}

void ConfirmDialog::show() {
    // 背景を暗くする（オーバーレイ効果）
    // 簡易的に黒い半透明の矩形を描画
    for (int i = 0; i < tft->height(); i += 4) {
        for (int j = 0; j < tft->width(); j += 4) {
            tft->drawPixel(j, i, OVERLAY_COLOR);
            tft->drawPixel(j+1, i, OVERLAY_COLOR);
            tft->drawPixel(j, i+1, OVERLAY_COLOR);
            tft->drawPixel(j+1, i+1, OVERLAY_COLOR);
        }
    }
    
    drawDialog();
}

void ConfirmDialog::drawDialog() {
    // ダイアログの背景（白）
    tft->fillRoundRect(x, y, width, height, 12, TFT_WHITE);
    
    // 枠線
    tft->drawRoundRect(x, y, width, height, 12, tft->color565(200, 200, 200));
    
    // タイトル背景
    tft->fillRoundRect(x, y, width, 40, 12, tft->color565(33, 150, 243));
    tft->fillRect(x, y + 20, width, 20, tft->color565(33, 150, 243));
    
    // タイトルテキスト
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    
    // タイトルを中央揃え
    int32_t titleWidth = tft->textWidth(title);
    tft->setCursor(x + (width - titleWidth) / 2, y + 12);
    tft->print(title);
    
    // メッセージ
    tft->setTextColor(TFT_BLACK);
    tft->setFont(&fonts::lgfxJapanGothic_12);
    
    // メッセージを中央揃え
    int32_t messageWidth = tft->textWidth(message);
    tft->setCursor(x + (width - messageWidth) / 2, y + 60);
    tft->print(message);
    
    // デフォルトフォントに戻す
    tft->setFont(nullptr);
    
    // ボタンを描画
    yesButton->draw();
    noButton->draw();
}

bool ConfirmDialog::handleTouch(int32_t touchX, int32_t touchY, bool pressed) {
    // ダイアログ外のタッチは無視
    if (touchX < x || touchX > x + width || touchY < y || touchY > y + height) {
        return false;
    }
    
    // ボタンのタッチ処理
    bool handled = false;
    
    if (yesButton->handleTouch(touchX, touchY, pressed)) {
        handled = true;
    } else if (noButton->handleTouch(touchX, touchY, pressed)) {
        handled = true;
    }
    
    return handled;
}

void ConfirmDialog::hide() {
    // ダイアログを非表示にする
    // 呼び出し元で画面を再描画する必要がある
}