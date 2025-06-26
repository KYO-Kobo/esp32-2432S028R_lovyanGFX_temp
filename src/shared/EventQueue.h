#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Events.h"

class EventQueue {
private:
    QueueHandle_t queue;
    const size_t queue_size;
    
public:
    EventQueue(size_t size = 32) : queue_size(size) {
        queue = xQueueCreate(queue_size, sizeof(Event));
    }
    
    ~EventQueue() {
        if (queue != nullptr) {
            vQueueDelete(queue);
        }
    }
    
    // イベントを送信（ノンブロッキング）
    bool send(const Event& event) {
        return xQueueSend(queue, &event, 0) == pdTRUE;
    }
    
    // イベントを送信（ブロッキング）
    bool sendWait(const Event& event, TickType_t wait_ticks = portMAX_DELAY) {
        return xQueueSend(queue, &event, wait_ticks) == pdTRUE;
    }
    
    // イベントを受信（ノンブロッキング）
    bool receive(Event& event) {
        return xQueueReceive(queue, &event, 0) == pdTRUE;
    }
    
    // イベントを受信（ブロッキング）
    bool receiveWait(Event& event, TickType_t wait_ticks = portMAX_DELAY) {
        return xQueueReceive(queue, &event, wait_ticks) == pdTRUE;
    }
    
    // キューが空かチェック
    bool isEmpty() const {
        return uxQueueMessagesWaiting(queue) == 0;
    }
    
    // キューに入っているイベント数を取得
    size_t getCount() const {
        return uxQueueMessagesWaiting(queue);
    }
};

// グローバルイベントキュー（Core1 → Core0）
extern EventQueue* g_touchEventQueue;

#endif // EVENT_QUEUE_H