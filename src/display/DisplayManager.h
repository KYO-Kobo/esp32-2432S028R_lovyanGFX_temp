#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "../shared/Events.h"
#include "../shared/EventQueue.h"

// 前方宣言
namespace lgfx {
    namespace v1 {
        class LGFX_Device;
    }
}
using LGFX = lgfx::v1::LGFX_Device;

class DisplayManager {
private:
    LGFX* tft;
    bool dirty;
    TouchEvent lastTouch;
    
    // 表示更新フラグ
    bool needsRedraw;
    
    // 最後に表示した座標（変更検出用）
    int32_t lastDisplayedX;
    int32_t lastDisplayedY;
    int32_t lastDisplayedRawX;
    int32_t lastDisplayedRawY;
    
public:
    DisplayManager(LGFX* display);
    
    // 初期化
    void init();
    
    // 画面更新（Core0のメインループから呼ばれる）
    void update();
    
    // イベント処理
    void handleEvent(const Event& event);
    
    // 画面の再描画
    void redraw();
    
    // タッチ位置の表示更新
    void updateTouchDisplay(int32_t x, int32_t y, int32_t raw_x, int32_t raw_y);
    
    // タッチポイントの描画
    void drawTouchPoint(int32_t x, int32_t y);
    
    // ステータス表示
    void updateStatus(const char* message);
};

#endif // DISPLAY_MANAGER_H