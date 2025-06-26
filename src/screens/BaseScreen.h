#ifndef BASE_SCREEN_H
#define BASE_SCREEN_H

#include "../shared/Events.h"

// 前方宣言
namespace lgfx {
    namespace v1 {
        class LGFX_Device;
    }
}
using LGFX = lgfx::v1::LGFX_Device;

// 画面IDの定義
enum ScreenID {
    SCREEN_HOME = 0,
    SCREEN_MENU,
    SCREEN_SETTINGS,
    SCREEN_CALIBRATION,
    SCREEN_INFO,
    SCREEN_COUNT
};

// 画面遷移アニメーションの種類
enum TransitionType {
    TRANSITION_NONE = 0,
    TRANSITION_SLIDE_UP,
    TRANSITION_SLIDE_DOWN,
    TRANSITION_SLIDE_LEFT,
    TRANSITION_SLIDE_RIGHT,
    TRANSITION_FADE
};

// 基底画面クラス
class BaseScreen {
protected:
    LGFX* tft;
    ScreenID screenId;
    bool needsRedraw;
    
public:
    BaseScreen(LGFX* display, ScreenID id);
    virtual ~BaseScreen() {}
    
    // 必須実装メソッド
    virtual void init() = 0;                    // 画面初期化
    virtual void draw() = 0;                    // 画面描画
    virtual void update() = 0;                  // 状態更新
    virtual void handleEvent(const Event& event) = 0;  // イベント処理
    
    // オプションメソッド
    virtual void onEnter() {}                   // 画面に入る時
    virtual void onExit() {}                    // 画面から出る時
    virtual bool canTransitionTo(ScreenID nextScreen) { return true; }
    
    // ジェスチャー処理（オーバーライド可能）
    virtual void onSwipeUp() {}
    virtual void onSwipeDown() {}
    virtual void onSwipeLeft() {}
    virtual void onSwipeRight() {}
    
    // 共通メソッド
    ScreenID getId() const { return screenId; }
    bool isNeedsRedraw() const { return needsRedraw; }
    void setNeedsRedraw(bool needs) { needsRedraw = needs; }
};

#endif // BASE_SCREEN_H