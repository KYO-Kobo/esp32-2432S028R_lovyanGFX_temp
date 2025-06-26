#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "ModernButton.h"
#include <Arduino.h>

ModernButton::ModernButton(LGFX* display, int16_t x, int16_t y, uint16_t w, uint16_t h, const std::string& text)
    : tft(display), x(x), y(y), width(w), height(h), 
      state(BUTTON_NORMAL), text(text), style(), 
      enabled(true), visible(true), needsRedraw(true) {
}

void ModernButton::draw() {
    if (!visible || !tft) return;
    
    // 影を描画（ボタンが押されていない時のみ）
    if (state != BUTTON_PRESSED && style.shadowOffset > 0) {
        drawShadow();
    }
    
    // ボタン背景を描画
    drawBackground();
    
    // テキストを描画
    drawText();
    
    needsRedraw = false;
}

void ModernButton::drawShadow() {
    // 影の色を半透明にするため、ブレンド効果をシミュレート
    int16_t shadowX = x + style.shadowOffset;
    int16_t shadowY = y + style.shadowOffset;
    
    // 角丸の影
    if (style.cornerRadius > 0) {
        tft->fillRoundRect(shadowX, shadowY, width, height, style.cornerRadius, style.shadowColor);
    } else {
        tft->fillRect(shadowX, shadowY, width, height, style.shadowColor);
    }
}

void ModernButton::drawBackground() {
    uint16_t color = getCurrentColor();
    int16_t drawX = x;
    int16_t drawY = y;
    
    // 押された時は少しずらす（押し込み効果）
    if (state == BUTTON_PRESSED) {
        drawX += 2;
        drawY += 2;
    }
    
    // 角丸の背景
    if (style.cornerRadius > 0) {
        tft->fillRoundRect(drawX, drawY, width, height, style.cornerRadius, color);
        
        // ボーダーを描画
        if (style.borderWidth > 0) {
            tft->drawRoundRect(drawX, drawY, width, height, style.cornerRadius, style.borderColor);
        }
    } else {
        tft->fillRect(drawX, drawY, width, height, color);
        
        // ボーダーを描画
        if (style.borderWidth > 0) {
            tft->drawRect(drawX, drawY, width, height, style.borderColor);
        }
    }
    
    // グラデーション効果（上部にハイライト）
    if (state == BUTTON_NORMAL && enabled) {
        // 上部に薄い白のラインでハイライト効果
        uint16_t highlightColor = tft->color565(255, 255, 255);
        tft->drawFastHLine(drawX + style.cornerRadius, drawY + 1, width - 2 * style.cornerRadius, highlightColor);
        tft->drawFastHLine(drawX + style.cornerRadius, drawY + 2, width - 2 * style.cornerRadius, highlightColor);
    }
}

void ModernButton::drawText() {
    if (text.empty()) return;
    
    int16_t drawX = x;
    int16_t drawY = y;
    
    // 押された時は少しずらす
    if (state == BUTTON_PRESSED) {
        drawX += 2;
        drawY += 2;
    }
    
    // テキストの中央配置を計算
    int16_t textX, textY;
    getTextBounds(textX, textY);
    
    // テキスト色を設定
    uint16_t textColor = enabled ? style.textColor : tft->color565(128, 128, 128);
    tft->setTextColor(textColor);
    tft->setTextSize(style.fontSize);
    
    // テキストを描画
    tft->setCursor(drawX + textX, drawY + textY);
    tft->print(text.c_str());
}

uint16_t ModernButton::getCurrentColor() const {
    if (!enabled) {
        return style.disabledColor;
    }
    
    switch (state) {
        case BUTTON_PRESSED:
            return style.pressedColor;
        case BUTTON_NORMAL:
        default:
            return style.normalColor;
    }
}

void ModernButton::getTextBounds(int16_t& tx, int16_t& ty) {
    // テキストサイズを計算（簡易版）
    int16_t textWidth = text.length() * 6 * style.fontSize;  // 各文字約6ピクセル
    int16_t textHeight = 8 * style.fontSize;  // 高さ約8ピクセル
    
    // 中央配置の計算
    tx = (width - textWidth) / 2;
    ty = (height - textHeight) / 2;
    
    // 最小マージンを確保
    if (tx < 4) tx = 4;
    if (ty < 4) ty = 4;
}

bool ModernButton::handleTouch(int16_t touchX, int16_t touchY, bool touching) {
    if (!enabled || !visible) return false;
    
    bool wasPressed = (state == BUTTON_PRESSED);
    bool isInside = contains(touchX, touchY);
    
    if (touching && isInside) {
        // タッチ中でボタン内
        if (state != BUTTON_PRESSED) {
            state = BUTTON_PRESSED;
            needsRedraw = true;
            draw();
        }
    } else {
        // タッチ終了またはボタン外
        if (state == BUTTON_PRESSED) {
            state = BUTTON_NORMAL;
            needsRedraw = true;
            draw();
            
            // クリックイベント発生（ボタン内でリリースされた場合）
            if (!touching && isInside && wasPressed && onClick) {
                onClick();
                return true;
            }
        }
    }
    
    return false;
}

bool ModernButton::contains(int16_t px, int16_t py) const {
    return px >= x && px < (x + width) && py >= y && py < (y + height);
}

void ModernButton::setText(const std::string& newText) {
    if (text != newText) {
        text = newText;
        needsRedraw = true;
    }
}

void ModernButton::setStyle(const ButtonStyle& newStyle) {
    style = newStyle;
    needsRedraw = true;
}

void ModernButton::setEnabled(bool enable) {
    if (enabled != enable) {
        enabled = enable;
        state = BUTTON_NORMAL;
        needsRedraw = true;
    }
}

void ModernButton::setVisible(bool show) {
    if (visible != show) {
        visible = show;
        if (!visible) {
            // ボタンを非表示にする時は背景色で塗りつぶす
            tft->fillRect(x, y, width + style.shadowOffset, height + style.shadowOffset, TFT_BLACK);
        } else {
            needsRedraw = true;
        }
    }
}

void ModernButton::setPosition(int16_t newX, int16_t newY) {
    if (x != newX || y != newY) {
        // 古い位置をクリア
        if (visible) {
            tft->fillRect(x, y, width + style.shadowOffset, height + style.shadowOffset, TFT_BLACK);
        }
        x = newX;
        y = newY;
        needsRedraw = true;
    }
}

void ModernButton::setSize(uint16_t newWidth, uint16_t newHeight) {
    if (width != newWidth || height != newHeight) {
        // 古いサイズをクリア
        if (visible) {
            tft->fillRect(x, y, width + style.shadowOffset, height + style.shadowOffset, TFT_BLACK);
        }
        width = newWidth;
        height = newHeight;
        needsRedraw = true;
    }
}