#ifndef MODERN_BUTTON_H
#define MODERN_BUTTON_H

#include <functional>
#include <string>

// 前方宣言
namespace lgfx {
    namespace v1 {
        class LGFX_Device;
    }
}
using LGFX = lgfx::v1::LGFX_Device;

// ボタンの状態
enum ButtonState {
    BUTTON_NORMAL = 0,
    BUTTON_PRESSED,
    BUTTON_DISABLED
};

// ボタンのスタイル設定
struct ButtonStyle {
    // 色設定（16ビットカラー）
    uint16_t normalColor;     // マテリアルブルー
    uint16_t pressedColor;    // ダークブルー
    uint16_t disabledColor;   // グレー
    uint16_t textColor;       // 白
    uint16_t shadowColor;     // 黒（影用）
    
    ButtonStyle() {
        normalColor = 0x2E7D;     // RGB565: Material Blue
        pressedColor = 0x1E38;    // RGB565: Darker Blue
        disabledColor = 0x9CD3;   // RGB565: Gray
        textColor = 0xFFFF;       // RGB565: White
        shadowColor = 0x0000;     // RGB565: Black
    }
    
    // サイズ・形状
    uint8_t cornerRadius = 8;            // 角丸の半径
    uint8_t shadowOffset = 3;            // 影のオフセット
    uint8_t borderWidth = 0;             // ボーダー幅（0でボーダーなし）
    uint16_t borderColor = 0xFFFF;       // ボーダー色
    
    // フォント
    uint8_t fontSize = 1;                // テキストサイズ
    bool useBoldFont = false;            // 太字フォント使用
    bool useJapaneseFont = true;         // 日本語フォント使用
};

class ModernButton {
private:
    LGFX* tft;
    
    // 位置とサイズ
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    
    // 状態
    ButtonState state;
    std::string text;
    ButtonStyle style;
    bool enabled;
    bool visible;
    
    // コールバック
    std::function<void()> onClick;
    
    // 内部状態
    bool needsRedraw;
    
public:
    // コンストラクタ
    ModernButton(LGFX* display, int16_t x, int16_t y, uint16_t w, uint16_t h, const std::string& text);
    
    // 描画
    void draw();
    void redraw() { needsRedraw = true; draw(); }
    
    // タッチ処理
    bool handleTouch(int16_t touchX, int16_t touchY, bool touching);
    
    // プロパティ設定
    void setText(const std::string& newText);
    void setStyle(const ButtonStyle& newStyle);
    void setEnabled(bool enable);
    void setVisible(bool show);
    void setPosition(int16_t newX, int16_t newY);
    void setSize(uint16_t newWidth, uint16_t newHeight);
    
    // コールバック設定
    void setOnClick(std::function<void()> callback) { onClick = callback; }
    
    // 状態取得
    bool isPressed() const { return state == BUTTON_PRESSED; }
    bool isEnabled() const { return enabled; }
    bool isVisible() const { return visible; }
    bool contains(int16_t px, int16_t py) const;
    
private:
    // 内部描画関数
    void drawBackground();
    void drawText();
    void drawShadow();
    uint16_t getCurrentColor() const;
    
    // 文字の中央配置計算
    void getTextBounds(int16_t& tx, int16_t& ty);
};

#endif // MODERN_BUTTON_H