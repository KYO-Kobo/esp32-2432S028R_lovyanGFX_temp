#ifndef TIME_SETTINGS_SCREEN_H
#define TIME_SETTINGS_SCREEN_H

#include "BaseScreen.h"
#include <memory>
#include <vector>
// 前方宣言
class ModernButton;

class TimeSettingsScreen : public BaseScreen {
private:
    std::vector<std::unique_ptr<ModernButton>> buttons;
    ModernButton* backButton = nullptr;
    ModernButton* yearButton = nullptr;
    ModernButton* monthButton = nullptr;
    ModernButton* dayButton = nullptr;
    ModernButton* timeButton = nullptr;

    int year = 2024;
    int month = 4;
    int day = 1;
    int hour = 12;
    int minute = 34;

    enum class TimeField {
        None,
        Year,
        Month,
        Day,
        Time
    };

    bool popupActive = false;
    bool popupNeedsRedraw = false;
    TimeField popupField = TimeField::None;

    int popupValue = 0;
    int popupMinValue = 0;
    int popupMaxValue = 0;
    bool popupWrap = false;

    int popupHourValue = 0;
    int popupMinuteValue = 0;

    struct Rect {
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
    };

    Rect popupRect;
    Rect popupValueRect;
    Rect popupIncrementRect;
    Rect popupDecrementRect;
    Rect popupHourIncrementRect;
    Rect popupHourDecrementRect;
    Rect popupMinuteIncrementRect;
    Rect popupMinuteDecrementRect;
    Rect popupOkRect;
public:
    TimeSettingsScreen(LGFX* display);
    void init() override;
    void draw() override;
    void update() override;
    void handleEvent(const Event& event) override;
    void onEnter() override;
    void onExit() override;
private:
    void createButtons();
    void refreshButtonLabels();
    void openPopup(TimeField field);
    void closePopup(bool applyChanges);
    void drawPopup();
    void drawPopupSingleValue(const char* title, int value);
    void drawPopupTime();
    void handlePopupTouch(const Event& event);
    bool isPointInside(const Rect& rect, int32_t x, int32_t y) const;
    void adjustValue(int& value, int minValue, int maxValue, int delta, bool wrap);
    void syncPopupToField();
    void applyPopupToField();
    static Rect makeRect(int16_t x, int16_t y, int16_t w, int16_t h);
};

#endif // TIME_SETTINGS_SCREEN_H
