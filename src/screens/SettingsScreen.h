#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "BaseScreen.h"

class SettingsScreen : public BaseScreen {
private:
    // タッチ開始位置（スワイプ検出用）
    int32_t touchStartX;
    int32_t touchStartY;
    uint32_t touchStartTime;
    bool isTouching;
    
    // 設定項目
    int brightness;
    bool touchSound;
    
public:
    SettingsScreen(LGFX* display);
    
    // BaseScreenの実装
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    
    // 画面遷移時の処理
    void onEnter() override;
    void onExit() override;
    
    // 全方向のスワイプでホームに戻る
    void onSwipeUp() override;
    void onSwipeDown() override;
    void onSwipeLeft() override;
    void onSwipeRight() override;
    
private:
    // スワイプジェスチャーの検出
    void detectSwipeGesture(int32_t endX, int32_t endY);
    
    // ホーム画面に戻る
    void returnToHome();
};

#endif // SETTINGS_SCREEN_H