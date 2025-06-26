#ifndef CORE1_MANAGER_H
#define CORE1_MANAGER_H

#include "../input/TouchManager.h"

// 前方宣言
namespace lgfx {
    namespace v1 {
        class LGFX_Device;
    }
}
using LGFX = lgfx::v1::LGFX_Device;

class Core1Manager {
private:
    LGFX* tft;
    TouchManager* touchManager;
    TaskHandle_t touchTaskHandle;
    
public:
    Core1Manager(LGFX* display);
    ~Core1Manager();
    
    // Core 1の初期化
    void init();
    
    // タスクの開始
    void startTasks();
    
    // タッチ処理タスク（static関数）
    static void touchTask(void* parameter);
    
private:
    // タスクの実際の処理
    void runTouchTask();
};

#endif // CORE1_MANAGER_H