#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "TouchManager.h"
#include <Arduino.h>

TouchManager::TouchManager(LGFX* display) 
    : tft(display), touching(false), state(TOUCH_IDLE), touchStartTime(0),
      touchStartX(0), touchStartY(0) {
    lastTouch = {EVENT_NONE, 0, 0, 0, 0, 0, 0};
}

void TouchManager::init() {
    // タッチパネルの初期化は既にmain.cppで行われているため、ここでは状態のみ初期化
    state = TOUCH_IDLE;
    touching = false;
}

void TouchManager::update() {
    processTouchInput();
}

void TouchManager::processTouchInput() {
    int32_t x, y;
    
    // getTouch()でタッチ検出（より効率的）
    if (tft->getTouch(&x, &y)) {
        // Raw値を取得
        lgfx::touch_point_t tp;
        tft->getTouchRaw(&tp, 1);
        int32_t raw_x = tp.x;
        int32_t raw_y = tp.y;
        
        switch (state) {
            case TOUCH_IDLE:
                // 新しいタッチの開始
                state = TOUCH_PRESSED;
                touchStartTime = millis();
                touchStartX = x;
                touchStartY = y;
                sendTouchEvent(EVENT_TOUCH_DOWN, x, y, raw_x, raw_y);
                break;
                
            case TOUCH_PRESSED:
            case TOUCH_MOVING:
                // タッチ移動の検出（閾値を5ピクセルに増加）
                if (abs(x - lastTouch.x) > 5 || abs(y - lastTouch.y) > 5) {
                    state = TOUCH_MOVING;
                    sendTouchEvent(EVENT_TOUCH_MOVE, x, y, raw_x, raw_y);
                }
                break;
                
            case TOUCH_RELEASED:
                // リリース後に再度タッチされた
                state = TOUCH_PRESSED;
                touchStartTime = millis();
                touchStartX = x;
                touchStartY = y;
                sendTouchEvent(EVENT_TOUCH_DOWN, x, y, raw_x, raw_y);
                break;
        }
        
        lastTouch.x = x;
        lastTouch.y = y;
        touching = true;
        
    } else {
        // タッチされていない
        if (touching) {
            // タッチが終了した
            state = TOUCH_RELEASED;
            sendTouchEvent(EVENT_TOUCH_UP, lastTouch.x, lastTouch.y, 0, 0);
            
            // ジェスチャー検出
            detectGesture(touchStartX, touchStartY, lastTouch.x, lastTouch.y);
            
            touching = false;
            state = TOUCH_IDLE;
        }
    }
}

void TouchManager::detectGesture(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    // ジェスチャー検出の実装（将来的な拡張用）
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    int32_t distance = sqrt(dx * dx + dy * dy);
    
    if (distance > 50) {  // 最小スワイプ距離
        GestureEvent::Direction direction = GestureEvent::GESTURE_NONE;
        
        if (abs(dx) > abs(dy)) {
            // 水平方向のスワイプ
            direction = (dx > 0) ? GestureEvent::GESTURE_RIGHT : GestureEvent::GESTURE_LEFT;
        } else {
            // 垂直方向のスワイプ
            direction = (dy > 0) ? GestureEvent::GESTURE_DOWN : GestureEvent::GESTURE_UP;
        }
        
        if (direction != GestureEvent::GESTURE_NONE && g_touchEventQueue) {
            Event event;
            event.type = EVENT_GESTURE_SWIPE;
            event.data.gesture.type = EVENT_GESTURE_SWIPE;
            event.data.gesture.direction = direction;
            event.data.gesture.start_x = x1;
            event.data.gesture.start_y = y1;
            event.data.gesture.end_x = x2;
            event.data.gesture.end_y = y2;
            event.data.gesture.duration_ms = millis() - touchStartTime;
            
            g_touchEventQueue->send(event);
        }
    }
}

void TouchManager::sendTouchEvent(EventType type, int32_t x, int32_t y, int32_t raw_x, int32_t raw_y) {
    if (g_touchEventQueue) {
        Event event;
        event.type = type;
        event.data.touch.type = type;
        event.data.touch.x = x;
        event.data.touch.y = y;
        event.data.touch.raw_x = raw_x;
        event.data.touch.raw_y = raw_y;
        event.data.touch.timestamp = millis();
        event.data.touch.pressure = 0;  // XPT2046は圧力検出をサポートしていない
        
        g_touchEventQueue->send(event);
    }
}