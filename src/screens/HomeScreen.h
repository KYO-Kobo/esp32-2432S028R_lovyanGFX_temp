#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "BaseScreen.h"
#include <memory>

class HomeScreen : public BaseScreen {
private:
    // 最小構成：状態は保持しない
    
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
};

#endif // HOME_SCREEN_H