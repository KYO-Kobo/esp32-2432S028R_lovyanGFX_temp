#ifndef INPUT_SETTINGS_SCREEN_H
#define INPUT_SETTINGS_SCREEN_H

#include "BaseScreen.h"
#include <memory>
#include <vector>
#include <string>
// 前方宣言
class ModernButton;

class InputSettingsScreen : public BaseScreen {
private:
    std::vector<std::unique_ptr<ModernButton>> buttons;
    bool sdAvailable = false;          // SDカード初期化成功したか
    int mp3Count = 0;                  // mp3ファイル数
    std::string sdErrorMsg;            // SD失敗時のエラーメッセージ
public:
    InputSettingsScreen(LGFX* display);
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    void onEnter() override;
    void onExit() override;
private:
    void createButtons();
    void checkSDAndCountMp3();    // SD初期化・mp3数取得
};

#endif // INPUT_SETTINGS_SCREEN_H
