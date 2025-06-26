#ifndef CORE0_MANAGER_H
#define CORE0_MANAGER_H

#include "../display/DisplayManager.h"

// 前方宣言
namespace lgfx {
    namespace v1 {
        class LGFX_Device;
    }
}
using LGFX = lgfx::v1::LGFX_Device;

class Core0Manager {
private:
    LGFX* tft;
    DisplayManager* displayManager;
    TaskHandle_t displayTaskHandle;
    
public:
    Core0Manager(LGFX* display);
    ~Core0Manager();
    
    // Core 0の初期化
    void init();
    
    // タスクの開始
    void startTasks();
    
    // 表示更新タスク（static関数）
    static void displayTask(void* parameter);
    
private:
    // タスクの実際の処理
    void runDisplayTask();
};

#endif // CORE0_MANAGER_H