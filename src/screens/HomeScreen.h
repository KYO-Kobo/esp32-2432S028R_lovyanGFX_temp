#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "BaseScreen.h"
#include <memory>

// 前方宣言
class Label;

class HomeScreen : public BaseScreen {
private:
    // 座標表示用の変数
    int32_t lastDisplayedX;
    int32_t lastDisplayedY;
    int32_t lastDisplayedRawX;
    int32_t lastDisplayedRawY;
    
    // タッチ開始位置（スワイプ検出用）
    int32_t touchStartX;
    int32_t touchStartY;
    uint32_t touchStartTime;
    bool isTouching;
    
    // リセットメッセージ表示用
    std::unique_ptr<Label> resetMessageLabel;
    uint32_t resetMessageStartTime;
    static constexpr uint32_t RESET_MESSAGE_DURATION = 3000;  // 3秒間表示
    
public:
    HomeScreen(LGFX* display);
    
    // BaseScreenの実装
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    
    // 画面遷移時の処理
    void onEnter() override;
    void onExit() override;
    
    // ジェスチャー処理
    void onSwipeUp() override;
    
private:
    // タッチ座標表示の更新
    void updateTouchDisplay(int32_t x, int32_t y, int32_t raw_x, int32_t raw_y);
    
    // スワイプジェスチャーの検出
    void detectSwipeGesture(int32_t endX, int32_t endY);
    
public:
    // リセットメッセージの表示
    void showResetMessage();
};

#endif // HOME_SCREEN_H