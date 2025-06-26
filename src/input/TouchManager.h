#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H

#include "../shared/Events.h"
#include "../shared/EventQueue.h"

// 前方宣言
namespace lgfx {
    namespace v1 {
        class LGFX_Device;
    }
}
using LGFX = lgfx::v1::LGFX_Device;

class TouchManager {
private:
    LGFX* tft;
    bool touching;
    TouchEvent lastTouch;
    uint32_t touchStartTime;
    
    // タッチ状態管理
    enum TouchState {
        TOUCH_IDLE,
        TOUCH_PRESSED,
        TOUCH_MOVING,
        TOUCH_RELEASED
    } state;
    
public:
    TouchManager(LGFX* display);
    
    // 初期化
    void init();
    
    // タッチ処理（Core1のメインループから呼ばれる）
    void update();
    
    // タッチイベントを検出して送信
    void processTouchInput();
    
    // ジェスチャー検出
    void detectGesture(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    
private:
    // イベントをキューに送信
    void sendTouchEvent(EventType type, int32_t x, int32_t y, int32_t raw_x, int32_t raw_y);
};

#endif // TOUCH_MANAGER_H