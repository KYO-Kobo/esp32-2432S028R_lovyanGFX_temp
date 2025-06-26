#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "BaseScreen.h"
#include "../shared/Events.h"
#include <memory>
#include <map>

// 前方宣言
class HomeScreen;
class MenuScreen;
class SettingsScreen;
class InfoScreen;

class ScreenManager {
private:
    LGFX* tft;
    BaseScreen* currentScreen;
    std::map<ScreenID, std::unique_ptr<BaseScreen>> screens;
    
    // 画面遷移中フラグ
    bool isTransitioning;
    
public:
    ScreenManager(LGFX* display);
    ~ScreenManager();
    
    // 初期化
    void init();
    
    // 画面の登録
    void registerScreen(ScreenID id, std::unique_ptr<BaseScreen> screen);
    
    // 画面遷移
    bool transitionTo(ScreenID screenId, TransitionType transition = TRANSITION_NONE);
    
    // 現在の画面取得
    BaseScreen* getCurrentScreen() { return currentScreen; }
    ScreenID getCurrentScreenId() const;
    
    // イベント処理
    void handleEvent(const Event& event);
    
    // 更新と描画
    void update();
    
private:
    // 画面遷移アニメーション
    void performTransition(BaseScreen* fromScreen, BaseScreen* toScreen, TransitionType transition);
    
    // スワイプイベントの処理
    void handleSwipeEvent(const Event& event);
};

#endif // SCREEN_MANAGER_H