#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "BaseScreen.h"

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
};

#endif // HOME_SCREEN_H