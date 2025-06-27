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
    
    // ボタン全体の領域をクリア（影も含む）
    // 注: ダイアログなど白い背景で使用する場合は、このクリア処理をスキップ
    // int16_t clearSize = style.shadowOffset + 2;  // 少し余裕を持たせる
    // tft->fillRect(x - 1, y - 1, width + clearSize + 2, height + clearSize + 2, TFT_BLACK);
    
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
    uint16_t drawWidth = width;
    uint16_t drawHeight = height;
    
    // 押された時はサイズを縮小（中心を保つ）
    if (state == BUTTON_PRESSED) {
        const uint8_t shrinkAmount = 4;  // 縮小量（左右上下それぞれ2ピクセルずつ）
        drawX += shrinkAmount / 2;
        drawY += shrinkAmount / 2;
        drawWidth -= shrinkAmount;
        drawHeight -= shrinkAmount;
    }
    
    // 角丸の背景
    if (style.cornerRadius > 0) {
        tft->fillRoundRect(drawX, drawY, drawWidth, drawHeight, style.cornerRadius, color);
        
        // ボーダーを描画
        if (style.borderWidth > 0) {
            tft->drawRoundRect(drawX, drawY, drawWidth, drawHeight, style.cornerRadius, style.borderColor);
        }
    } else {
        tft->fillRect(drawX, drawY, drawWidth, drawHeight, color);
        
        // ボーダーを描画
        if (style.borderWidth > 0) {
            tft->drawRect(drawX, drawY, drawWidth, drawHeight, style.borderColor);
        }
    }
    
    // グラデーション効果（上部にハイライト）
    if (state == BUTTON_NORMAL && enabled) {
        // 上部に薄い白のラインでハイライト効果
        uint16_t highlightColor = tft->color565(255, 255, 255);
        tft->drawFastHLine(drawX + style.cornerRadius, drawY + 1, drawWidth - 2 * style.cornerRadius, highlightColor);
        tft->drawFastHLine(drawX + style.cornerRadius, drawY + 2, drawWidth - 2 * style.cornerRadius, highlightColor);
    }
}

void ModernButton::drawText() {
    if (text.empty()) return;
    
    int16_t drawX = x;
    int16_t drawY = y;
    uint16_t drawWidth = width;
    uint16_t drawHeight = height;
    
    // 押された時はサイズを縮小（中心を保つ）
    if (state == BUTTON_PRESSED) {
        const uint8_t shrinkAmount = 4;
        drawX += shrinkAmount / 2;
        drawY += shrinkAmount / 2;
        drawWidth -= shrinkAmount;
        drawHeight -= shrinkAmount;
    }
    
    // 日本語フォントを設定（日本語が含まれている場合）
    bool hasJapanese = false;
    for (size_t i = 0; i < text.length(); i++) {
        if ((unsigned char)text[i] >= 0x80) {
            hasJapanese = true;
            break;
        }
    }
    
    if (hasJapanese && style.useJapaneseFont) {
        // 日本語フォントを使用
        tft->setFont(&fonts::lgfxJapanGothic_12);
    } else {
        // デフォルトフォント
        tft->setFont(nullptr);
        tft->setTextSize(style.fontSize);
    }
    
    // テキストの中央配置を計算（縮小時は新しいサイズで計算）
    int16_t textX, textY;
    if (state == BUTTON_PRESSED) {
        // 縮小したボタンサイズでテキスト位置を再計算
        getTextBoundsForSize(textX, textY, drawWidth, drawHeight);
    } else {
        getTextBounds(textX, textY);
    }
    
    // テキスト色を設定
    uint16_t textColor = enabled ? style.textColor : tft->color565(128, 128, 128);
    tft->setTextColor(textColor);
    
    // テキストを描画
    tft->setCursor(drawX + textX, drawY + textY);
    tft->print(text.c_str());
    
    // フォントをリセット
    if (hasJapanese && style.useJapaneseFont) {
        tft->setFont(nullptr);
        tft->setTextSize(1);
    }
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
    getTextBoundsForSize(tx, ty, width, height);
}

void ModernButton::getTextBoundsForSize(int16_t& tx, int16_t& ty, uint16_t buttonWidth, uint16_t buttonHeight) {
    // 日本語フォントかどうかを判定
    bool hasJapanese = false;
    for (size_t i = 0; i < text.length(); i++) {
        if ((unsigned char)text[i] >= 0x80) {
            hasJapanese = true;
            break;
        }
    }
    
    // 現在のフォント設定を保存
    const lgfx::v1::IFont* originalFont = tft->getFont();
    
    // 実際に使用するフォントを設定して正確な幅を測定
    if (hasJapanese && style.useJapaneseFont) {
        tft->setFont(&fonts::lgfxJapanGothic_12);
    } else {
        tft->setFont(nullptr);
        tft->setTextSize(style.fontSize);
    }
    
    // LovyanGFXのtextWidth()とfontHeight()を使用して正確なサイズを取得
    int32_t textWidth = tft->textWidth(text.c_str());
    int32_t textHeight = tft->fontHeight();
    
    // フォント設定を元に戻す
    tft->setFont(originalFont);
    
    // 中央配置の計算（指定されたボタンサイズに対して）
    tx = (buttonWidth - textWidth) / 2;
    ty = (buttonHeight - textHeight) / 2;
    
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