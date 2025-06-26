#ifndef EVENTS_H
#define EVENTS_H

#include <cstdint>

// イベントタイプ定義
enum EventType {
    EVENT_NONE = 0,
    EVENT_TOUCH_DOWN,
    EVENT_TOUCH_UP,
    EVENT_TOUCH_MOVE,
    EVENT_TOUCH_DRAG,
    EVENT_GESTURE_SWIPE,
    EVENT_DISPLAY_UPDATE,
    EVENT_SYSTEM_STATUS
};

// タッチイベントデータ
struct TouchEvent {
    EventType type;
    int32_t x;
    int32_t y;
    int32_t raw_x;
    int32_t raw_y;
    uint32_t timestamp;
    uint8_t pressure;
};

// ジェスチャーイベントデータ
struct GestureEvent {
    EventType type;
    enum Direction {
        GESTURE_NONE,
        GESTURE_UP,
        GESTURE_DOWN,
        GESTURE_LEFT,
        GESTURE_RIGHT
    } direction;
    int32_t start_x;
    int32_t start_y;
    int32_t end_x;
    int32_t end_y;
    uint32_t duration_ms;
};

// 汎用イベント構造体
struct Event {
    EventType type;
    union {
        TouchEvent touch;
        GestureEvent gesture;
    } data;
};

#endif // EVENTS_H