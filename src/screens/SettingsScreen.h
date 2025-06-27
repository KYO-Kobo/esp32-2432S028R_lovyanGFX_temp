#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "BaseScreen.h"
#include <memory>
#include <vector>

// 前方宣言
class ModernButton;
class ConfirmDialog;

class SettingsScreen : public BaseScreen {
private:
    // タッチ開始位置（スワイプ検出用）
    int32_t touchStartX;
    int32_t touchStartY;
    uint32_t touchStartTime;
    bool isTouching;
    
    // 設定項目
    int brightness;
    
    // UIコンポーネント
    std::vector<std::unique_ptr<ModernButton>> buttons;
    
    // 確認ダイアログ
    std::unique_ptr<ConfirmDialog> confirmDialog;
    bool showingDialog;
    
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
    
    // 全方向のスワイプでメニューに戻る
    void onSwipeUp() override;
    void onSwipeDown() override;
    void onSwipeLeft() override;
    void onSwipeRight() override;
    
private:
    // ボタンの作成
    void createButtons();
    
    // スワイプジェスチャーの検出
    void detectSwipeGesture(int32_t endX, int32_t endY);
    
    // メニュー画面に戻る
    void returnToMenu();
};

#endif // SETTINGS_SCREEN_H