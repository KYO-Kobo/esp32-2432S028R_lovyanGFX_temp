#include "Core0Manager.h"
#include <Arduino.h>

Core0Manager::Core0Manager(LGFX* display) : tft(display), displayManager(nullptr) {
}

Core0Manager::~Core0Manager() {
    if (displayManager) {
        delete displayManager;
    }
}

void Core0Manager::init() {
    // DisplayManagerを作成して初期化
    displayManager = new DisplayManager(tft);
    displayManager->init();
    
    Serial.println("Core 0: Display Manager initialized");
}

void Core0Manager::startTasks() {
    // Core 0に表示タスクを作成
    xTaskCreatePinnedToCore(
        displayTask,              // タスク関数
        "DisplayTask",            // タスク名
        8192,                     // スタックサイズ
        this,                     // パラメータ（thisポインタ）
        1,                        // 優先度（低）
        &displayTaskHandle,       // タスクハンドル
        0                         // Core 0に固定
    );
    
    Serial.println("Core 0: Display task started");
}

void Core0Manager::displayTask(void* parameter) {
    Core0Manager* manager = static_cast<Core0Manager*>(parameter);
    manager->runDisplayTask();
}

void Core0Manager::runDisplayTask() {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frameDelay = pdMS_TO_TICKS(16); // 約60FPS
    
    while (true) {
        // ディスプレイの更新
        if (displayManager) {
            displayManager->update();
        }
        
        // 60FPSを維持
        vTaskDelayUntil(&lastWakeTime, frameDelay);
    }
}