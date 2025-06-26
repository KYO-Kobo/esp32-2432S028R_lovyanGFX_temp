#include "Core1Manager.h"
#include <Arduino.h>

Core1Manager::Core1Manager(LGFX* display) : tft(display), touchManager(nullptr) {
}

Core1Manager::~Core1Manager() {
    if (touchManager) {
        delete touchManager;
    }
}

void Core1Manager::init() {
    // TouchManagerを作成して初期化
    touchManager = new TouchManager(tft);
    touchManager->init();
    
    Serial.println("Core 1: Touch Manager initialized");
}

void Core1Manager::startTasks() {
    // Core 1にタッチ処理タスクを作成
    xTaskCreatePinnedToCore(
        touchTask,                // タスク関数
        "TouchTask",              // タスク名
        4096,                     // スタックサイズ
        this,                     // パラメータ（thisポインタ）
        2,                        // 優先度（中）
        &touchTaskHandle,         // タスクハンドル
        1                         // Core 1に固定
    );
    
    Serial.println("Core 1: Touch task started");
}

void Core1Manager::touchTask(void* parameter) {
    Core1Manager* manager = static_cast<Core1Manager*>(parameter);
    manager->runTouchTask();
}

void Core1Manager::runTouchTask() {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t touchDelay = pdMS_TO_TICKS(10); // 100Hz（10ms）に変更
    
    while (true) {
        // タッチ入力の処理
        if (touchManager) {
            touchManager->update();
        }
        
        // 100Hzでサンプリング（応答性とCPU負荷のバランス）
        vTaskDelayUntil(&lastWakeTime, touchDelay);
    }
}