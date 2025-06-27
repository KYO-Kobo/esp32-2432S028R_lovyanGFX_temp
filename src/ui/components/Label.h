#ifndef LABEL_H
#define LABEL_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <string>

// 前方宣言
namespace lgfx {
    namespace v1 {
        class LGFX_Device;
    }
}
using LGFX = lgfx::v1::LGFX_Device;

class Label {
public:
    enum Alignment {
        LEFT,
        CENTER,
        RIGHT
    };

private:
    LGFX* tft;
    std::string text;
    int16_t x, y;
    uint16_t width, height;
    uint16_t textColor;
    uint16_t backgroundColor;
    bool hasBackground;
    Alignment alignment;
    bool useJapaneseFont;
    uint8_t fontSize;
    bool visible;

public:
    // コンストラクタ
    Label(LGFX* display, int16_t x, int16_t y, uint16_t w, uint16_t h, const std::string& text);
    
    // 描画
    void draw();
    
    // プロパティ設定
    void setText(const std::string& newText);
    void setTextColor(uint16_t color) { textColor = color; }
    void setBackgroundColor(uint16_t color) { backgroundColor = color; hasBackground = true; }
    void clearBackground() { hasBackground = false; }
    void setAlignment(Alignment align) { alignment = align; }
    void setJapaneseFont(bool enable) { useJapaneseFont = enable; }
    void setFontSize(uint8_t size) { fontSize = size; }
    void setVisible(bool show) { visible = show; }
    void setPosition(int16_t newX, int16_t newY) { x = newX; y = newY; }
    
    // 状態取得
    bool isVisible() const { return visible; }
    const std::string& getText() const { return text; }
    
    // 中央配置用のヘルパー
    void centerInScreen(LGFX* display);
    
    // 文字の長さに合わせて幅を調整
    void adjustWidthToText();
    
private:
    // テキストの描画位置を計算
    void calculateTextPosition(int16_t& textX, int16_t& textY);
};

#endif // LABEL_H