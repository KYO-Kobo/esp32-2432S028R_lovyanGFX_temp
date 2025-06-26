#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "ScreenManager.h"
#include "HomeScreen.h"
#include "SettingsScreen.h"
#include <Arduino.h>

ScreenManager::ScreenManager(LGFX* display) 
    : tft(display), currentScreen(nullptr), isTransitioning(false) {
}

ScreenManager::~ScreenManager() {
    // unique_ptrが自動的にメモリを解放
}

void ScreenManager::init() {
    // 画面を登録
    registerScreen(SCREEN_HOME, std::unique_ptr<BaseScreen>(new HomeScreen(tft)));
    registerScreen(SCREEN_SETTINGS, std::unique_ptr<BaseScreen>(new SettingsScreen(tft)));
    
    // ホーム画面から開始
    transitionTo(SCREEN_HOME);
}

void ScreenManager::registerScreen(ScreenID id, std::unique_ptr<BaseScreen> screen) {
    screens[id] = std::move(screen);
}

bool ScreenManager::transitionTo(ScreenID screenId, TransitionType transition) {
    if (isTransitioning) {
        return false;  // 遷移中は新しい遷移を受け付けない
    }
    
    auto it = screens.find(screenId);
    if (it == screens.end()) {
        Serial.printf("Screen %d not found\n", screenId);
        return false;
    }
    
    BaseScreen* nextScreen = it->second.get();
    
    // 同じ画面への遷移はスキップ
    if (currentScreen == nextScreen) {
        return false;
    }
    
    // 遷移可能かチェック
    if (currentScreen && !currentScreen->canTransitionTo(screenId)) {
        return false;
    }
    
    isTransitioning = true;
    
    // 画面遷移を実行
    performTransition(currentScreen, nextScreen, transition);
    
    // 現在の画面を更新
    if (currentScreen) {
        currentScreen->onExit();
    }
    
    currentScreen = nextScreen;
    currentScreen->onEnter();
    
    isTransitioning = false;
    
    Serial.printf("Transitioned to screen %d\n", screenId);
    return true;
}

ScreenID ScreenManager::getCurrentScreenId() const {
    if (currentScreen) {
        return currentScreen->getId();
    }
    return SCREEN_HOME;
}

void ScreenManager::handleEvent(const Event& event) {
    if (!currentScreen || isTransitioning) {
        return;
    }
    
    // スワイプイベントの特別処理
    if (event.type == EVENT_GESTURE_SWIPE || 
        (event.type == EVENT_TOUCH_UP && currentScreen)) {
        handleSwipeEvent(event);
    }
    
    // 現在の画面にイベントを渡す
    currentScreen->handleEvent(event);
}

void ScreenManager::update() {
    if (!currentScreen || isTransitioning) {
        return;
    }
    
    currentScreen->update();
    
    // 再描画が必要な場合
    if (currentScreen->isNeedsRedraw()) {
        currentScreen->draw();
    }
}

void ScreenManager::performTransition(BaseScreen* fromScreen, BaseScreen* toScreen, TransitionType transition) {
    // 簡単な遷移実装（将来的にアニメーションを追加可能）
    switch (transition) {
        case TRANSITION_SLIDE_UP:
            // TODO: スライドアップアニメーション
            tft->fillScreen(TFT_BLACK);
            break;
            
        case TRANSITION_SLIDE_DOWN:
            // TODO: スライドダウンアニメーション
            tft->fillScreen(TFT_BLACK);
            break;
            
        case TRANSITION_FADE:
            // TODO: フェードアニメーション
            tft->fillScreen(TFT_BLACK);
            break;
            
        default:
            // 即座に切り替え
            tft->fillScreen(TFT_BLACK);
            break;
    }
}

void ScreenManager::handleSwipeEvent(const Event& event) {
    if (!currentScreen) return;
    
    ScreenID currentId = currentScreen->getId();
    
    // ホーム画面での上スワイプ → 設定画面
    if (currentId == SCREEN_HOME && event.type == EVENT_TOUCH_UP) {
        // HomeScreenクラス内でスワイプ検出を行い、onSwipeUp()を呼ぶ
        // その結果、ここで画面遷移を実行
        HomeScreen* homeScreen = static_cast<HomeScreen*>(currentScreen);
        if (homeScreen) {
            // HomeScreenのhandleEventで処理される
        }
    }
    
    // 設定画面での任意方向スワイプ → ホーム画面
    if (currentId == SCREEN_SETTINGS) {
        SettingsScreen* settingsScreen = static_cast<SettingsScreen*>(currentScreen);
        if (settingsScreen) {
            // SettingsScreenのhandleEventで処理される
        }
    }
    
    // 画面のonSwipeXxx()メソッドが呼ばれた後、
    // DisplayManagerでScreenManagerのtransitionTo()を呼ぶ必要がある
}