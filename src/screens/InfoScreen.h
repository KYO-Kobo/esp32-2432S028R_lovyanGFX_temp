#ifndef INFO_SCREEN_H
#define INFO_SCREEN_H

#include "BaseScreen.h"
#include <memory>
#include <vector>

// 前方宣言
class ModernButton;

class InfoScreen : public BaseScreen {
private:
    // タッチ開始位置（スワイプ検出用）
    int32_t touchStartX;
    int32_t touchStartY;
    uint32_t touchStartTime;
    bool isTouching;
    
    // システム情報
    String boardName;
    String productName;
    String version;
    String chipId;
    String macAddress;
    uint32_t freeHeap;
    uint32_t totalHeap;
    uint32_t freePsram;
    uint32_t totalPsram;
    uint32_t flashSize;
    
    // UIコンポーネント
    std::vector<std::unique_ptr<ModernButton>> buttons;
    
public:
    InfoScreen(LGFX* display);
    
    // BaseScreenの実装
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    
    // 画面遷移時の処理
    void onEnter() override;
    void onExit() override;
    
    // スワイプで設定画面に戻る
    void onSwipeUp() override;
    void onSwipeDown() override;
    void onSwipeLeft() override;
    void onSwipeRight() override;
    
private:
    // ボタンの作成
    void createButtons();
    
    // システム情報の取得
    void updateSystemInfo();
    
    // スワイプジェスチャーの検出
    void detectSwipeGesture(int32_t endX, int32_t endY);
    
    // 設定画面に戻る
    void returnToSettings();
};

#endif // INFO_SCREEN_H