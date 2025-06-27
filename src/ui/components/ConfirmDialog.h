#ifndef CONFIRM_DIALOG_H
#define CONFIRM_DIALOG_H

#include <LovyanGFX.hpp>
#include "ModernButton.h"
#include <memory>
#include <functional>

class ConfirmDialog {
private:
    LGFX* tft;
    String title;
    String message;
    
    // ダイアログの位置とサイズ
    int16_t x, y, width, height;
    
    // ボタン
    std::unique_ptr<ModernButton> yesButton;
    std::unique_ptr<ModernButton> noButton;
    
    // コールバック
    std::function<void()> onYesCallback;
    std::function<void()> onNoCallback;
    
    // 背景を暗くするための設定
    static constexpr uint16_t OVERLAY_COLOR = 0x0000;  // 半透明の黒
    static constexpr uint8_t OVERLAY_ALPHA = 128;      // 透明度
    
public:
    ConfirmDialog(LGFX* display, const String& title, const String& message);
    
    // コールバック設定
    void setOnYes(std::function<void()> callback) { onYesCallback = callback; }
    void setOnNo(std::function<void()> callback) { onNoCallback = callback; }
    
    // 表示
    void show();
    
    // タッチ処理
    bool handleTouch(int32_t touchX, int32_t touchY, bool pressed);
    
    // 非表示（画面を再描画する必要がある）
    void hide();
    
private:
    void drawDialog();
    void createButtons();
};

#endif // CONFIRM_DIALOG_H