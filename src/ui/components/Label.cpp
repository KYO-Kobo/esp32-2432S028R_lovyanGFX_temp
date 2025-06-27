#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "Label.h"
#include <Arduino.h>


Label::Label(LGFX* display, int16_t x, int16_t y, uint16_t w, uint16_t h, const std::string& text)
    : tft(display), text(text), x(x), y(y), width(w), height(h),
      textColor(TFT_WHITE), backgroundColor(TFT_BLACK), hasBackground(false),
      alignment(CENTER), useJapaneseFont(false), fontSize(16), visible(true) {
    
    // 文字の長さに合わせて幅を自動調整（幅が0の場合）
    if (width == 0 && tft) {
        adjustWidthToText();
    }
}

void Label::draw() {
    if (!visible || !tft) return;
    
    // 背景を描画（設定されている場合）
    if (hasBackground) {
        // 角丸の背景
        tft->fillRoundRect(x, y, width, height, 8, backgroundColor);
        // 枠線を追加（少し明るい色で）
        uint16_t borderColor = tft->color565(100, 150, 200);  // ライトブルー
        tft->drawRoundRect(x, y, width, height, 8, borderColor);
    }
    
    // テキスト色設定（背景色も指定して透過を防ぐ）
    if (hasBackground) {
        tft->setTextColor(textColor, backgroundColor);
    } else {
        tft->setTextColor(textColor);
    }
    
    // フォント設定
    if (useJapaneseFont) {
        if (fontSize <= 12) {
            tft->setFont(&fonts::lgfxJapanGothic_12);
        } else {
            tft->setFont(&fonts::lgfxJapanGothic_16);
        }
    } else {
        tft->setFont(nullptr);
        tft->setTextSize(fontSize / 8);  // 8ピクセルが基本サイズ
    }
    
    // テキスト位置を計算
    int16_t textX, textY;
    calculateTextPosition(textX, textY);
    
    // テキストを描画
    tft->setCursor(textX, textY);
    tft->print(text.c_str());
    
    // フォントをリセット
    if (useJapaneseFont) {
        tft->setFont(nullptr);
        tft->setTextSize(1);
    }
}

void Label::setText(const std::string& newText) {
    if (text != newText) {
        text = newText;
        // 文字の長さに合わせて幅を再調整
        adjustWidthToText();
    }
}

void Label::centerInScreen(LGFX* display) {
    if (!display) return;
    
    // 画面の中央に配置
    x = (display->width() - width) / 2;
    y = (display->height() - height) / 2;
}

void Label::calculateTextPosition(int16_t& textX, int16_t& textY) {
    // フォントサイズに基づいてテキストのサイズを推定
    int32_t textWidth = 0;
    int32_t textHeight = fontSize;
    
    if (tft) {
        // 実際のテキスト幅を取得
        if (useJapaneseFont) {
            if (fontSize <= 12) {
                tft->setFont(&fonts::lgfxJapanGothic_12);
            } else {
                tft->setFont(&fonts::lgfxJapanGothic_16);
            }
        }
        
        textWidth = tft->textWidth(text.c_str());
        
        if (useJapaneseFont) {
            tft->setFont(nullptr);
        }
    }
    
    // 水平方向の配置
    switch (alignment) {
        case LEFT:
            textX = x + 5;  // 左端から5ピクセルのマージン
            break;
        case RIGHT:
            textX = x + width - textWidth - 5;  // 右端から5ピクセルのマージン
            break;
        case CENTER:
        default:
            textX = x + (width - textWidth) / 2;
            break;
    }
    
    // 垂直方向は中央に配置
    textY = y + (height - textHeight) / 2;
    
    // 最小値の確保
    if (textX < x) textX = x;
    if (textY < y) textY = y;
}

void Label::adjustWidthToText() {
    if (!tft || text.empty()) return;
    
    // フォントを設定
    if (useJapaneseFont) {
        if (fontSize <= 12) {
            tft->setFont(&fonts::lgfxJapanGothic_12);
        } else {
            tft->setFont(&fonts::lgfxJapanGothic_16);
        }
    } else {
        tft->setFont(nullptr);
        tft->setTextSize(fontSize / 8);
    }
    
    // テキストの実際の幅を取得
    int32_t textWidth = tft->textWidth(text.c_str());
    
    // 左右に余白を追加（パディング）
    const int padding = 10;
    width = textWidth + (padding * 2);
    
    // 高さも調整（上下にパディング）
    height = fontSize + (padding * 2);
    
    // フォントをリセット
    tft->setFont(nullptr);
    tft->setTextSize(1);
}