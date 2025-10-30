#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "TimeSettingsScreen.h"
#include <Arduino.h>
#include "../ui/components/ModernButton.h"
#include "../shared/EventQueue.h"
#include <memory>
#include <vector>
#include <functional>
#include <cstdio>

extern EventQueue* g_touchEventQueue;

TimeSettingsScreen::TimeSettingsScreen(LGFX* display)
    : BaseScreen(display, SCREEN_TIME_SETTINGS) {}

void TimeSettingsScreen::createButtons() {
    buttons.clear();
    ButtonStyle defaultStyle;
    defaultStyle.normalColor = tft->color565(55, 71, 79);   // Dark blue grey
    defaultStyle.pressedColor = tft->color565(38, 50, 56);
    defaultStyle.cornerRadius = 6;
    defaultStyle.shadowOffset = 2;
    defaultStyle.useJapaneseFont = true;

    auto makeButton = [&](int16_t x, int16_t y, uint16_t w, uint16_t h, const char* label, std::function<void()> onClick) {
        auto btn = std::unique_ptr<ModernButton>(new ModernButton(tft, x, y, w, h, label));
        btn->setStyle(defaultStyle);
        btn->setOnClick(onClick);
        buttons.push_back(std::move(btn));
        return buttons.back().get();
    };

    backButton = makeButton(
        tft->width() - 70, 10, 60, 30, "戻る", [this]() {
            closePopup(false);
            Event e;
            e.type = EVENT_SCREEN_CHANGE;
            e.data.screenChange.targetScreen = SCREEN_MENU;
            e.data.screenChange.transition = TRANSITION_SLIDE_RIGHT;
            if (g_touchEventQueue) g_touchEventQueue->send(e);
        }
    );

    yearButton = makeButton(10, 60, 90, 40, "", [this]() {
        openPopup(TimeField::Year);
    });

    monthButton = makeButton(110, 60, 90, 40, "", [this]() {
        openPopup(TimeField::Month);
    });

    dayButton = makeButton(210, 60, 90, 40, "", [this]() {
        openPopup(TimeField::Day);
    });

    timeButton = makeButton(10, 120, 190, 40, "", [this]() {
        openPopup(TimeField::Time);
    });

    refreshButtonLabels();
}

void TimeSettingsScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("時間設定");
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setCursor(10, 50);
    tft->println("日付");
    tft->setCursor(10, 110);
    tft->println("時刻");
    tft->setFont(nullptr);
    for (auto& button : buttons) {
        button->draw();
    }
    if (popupActive) {
        popupNeedsRedraw = true;
        drawPopup();
    }
}

void TimeSettingsScreen::draw() {
    if (needsRedraw) {
        init();
        needsRedraw = false;
    }
    if (popupActive && popupNeedsRedraw) {
        drawPopup();
    }
}

void TimeSettingsScreen::update() {
}

void TimeSettingsScreen::handleEvent(const Event& event) {
    if (popupActive) {
        handlePopupTouch(event);
        return;
    }
    switch (event.type) {
        case EVENT_TOUCH_DOWN:
        case EVENT_TOUCH_MOVE:
            for (auto& button : buttons) {
                button->handleTouch(event.data.touch.x, event.data.touch.y, true);
            }
            break;
        case EVENT_TOUCH_UP: {
            for (auto& button : buttons) {
                button->handleTouch(event.data.touch.x, event.data.touch.y, false);
            }
            break;
        }
        default:
            break;
    }
}

void TimeSettingsScreen::onEnter() {
    createButtons();
    needsRedraw = true;
}

void TimeSettingsScreen::onExit() {
}


void TimeSettingsScreen::refreshButtonLabels() {
    if (!yearButton || !monthButton || !dayButton || !timeButton) {
        return;
    }

    char buffer[16];

    snprintf(buffer, sizeof(buffer), "%04d年", year);
    yearButton->setText(buffer);

    snprintf(buffer, sizeof(buffer), "%02d月", month);
    monthButton->setText(buffer);

    snprintf(buffer, sizeof(buffer), "%02d日", day);
    dayButton->setText(buffer);

    snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
    timeButton->setText(buffer);

    // 再描画
    yearButton->draw();
    monthButton->draw();
    dayButton->draw();
    timeButton->draw();
}

void TimeSettingsScreen::openPopup(TimeField field) {
    popupField = field;
    popupActive = true;
    popupNeedsRedraw = true;
    popupRect = makeRect(40, 40, 240, 180);

    popupOkRect = makeRect(static_cast<int16_t>(popupRect.x + 70),
                           static_cast<int16_t>(popupRect.y + 130),
                           100,
                           40);

    popupValueRect = makeRect(static_cast<int16_t>(popupRect.x + 90),
                              static_cast<int16_t>(popupRect.y + 75),
                              60,
                              30);

    popupIncrementRect = makeRect(static_cast<int16_t>(popupRect.x + 90),
                                  static_cast<int16_t>(popupRect.y + 45),
                                  60,
                                  28);

    popupDecrementRect = makeRect(static_cast<int16_t>(popupRect.x + 90),
                                  static_cast<int16_t>(popupRect.y + 110),
                                  60,
                                  28);

    popupHourIncrementRect = makeRect(static_cast<int16_t>(popupRect.x + 40),
                                      static_cast<int16_t>(popupRect.y + 60),
                                      60,
                                      30);

    popupHourDecrementRect = makeRect(static_cast<int16_t>(popupRect.x + 40),
                                      static_cast<int16_t>(popupRect.y + 110),
                                      60,
                                      30);

    popupMinuteIncrementRect = makeRect(static_cast<int16_t>(popupRect.x + 140),
                                        static_cast<int16_t>(popupRect.y + 60),
                                        60,
                                        30);

    popupMinuteDecrementRect = makeRect(static_cast<int16_t>(popupRect.x + 140),
                                        static_cast<int16_t>(popupRect.y + 110),
                                        60,
                                        30);

    syncPopupToField();
    drawPopup();
}

void TimeSettingsScreen::closePopup(bool applyChanges) {
    if (!popupActive) {
        return;
    }

    if (applyChanges) {
        applyPopupToField();
        refreshButtonLabels();
    }

    popupActive = false;
    popupNeedsRedraw = false;
    popupField = TimeField::None;
    needsRedraw = true;
}

void TimeSettingsScreen::drawPopup() {
    if (!popupActive) {
        return;
    }

    const uint16_t overlayColor = tft->color565(33, 33, 33);
    const uint16_t borderColor = tft->color565(100, 181, 246);
    const uint16_t buttonColor = tft->color565(55, 71, 79);

    tft->fillRoundRect(popupRect.x - 4, popupRect.y - 4, popupRect.w + 8, popupRect.h + 8, 10, TFT_BLACK);
    tft->fillRoundRect(popupRect.x, popupRect.y, popupRect.w, popupRect.h, 8, overlayColor);
    tft->drawRoundRect(popupRect.x, popupRect.y, popupRect.w, popupRect.h, 8, borderColor);

    switch (popupField) {
        case TimeField::Year:
            drawPopupSingleValue("年", popupValue);
            break;
        case TimeField::Month:
            drawPopupSingleValue("月", popupValue);
            break;
        case TimeField::Day:
            drawPopupSingleValue("日", popupValue);
            break;
        case TimeField::Time:
            drawPopupTime();
            break;
        default:
            break;
    }

    auto drawRectButton = [&](const Rect& rect, const char* label) {
        tft->fillRoundRect(rect.x, rect.y, rect.w, rect.h, 6, buttonColor);
        tft->drawRoundRect(rect.x, rect.y, rect.w, rect.h, 6, borderColor);
        tft->setTextColor(TFT_WHITE);
        tft->setFont(&fonts::lgfxJapanGothic_12);
        int16_t tx = rect.x + (rect.w / 2) - (tft->textWidth(label) / 2);
        int16_t ty = rect.y + (rect.h / 2) - (tft->fontHeight() / 2);
        tft->setCursor(tx, ty);
        tft->print(label);
        tft->setFont(nullptr);
    };

    drawRectButton(popupOkRect, "OK");

    if (popupField != TimeField::Time) {
        drawRectButton(popupIncrementRect, "▲");
        drawRectButton(popupDecrementRect, "▼");
    } else {
        drawRectButton(popupHourIncrementRect, "▲");
        drawRectButton(popupHourDecrementRect, "▼");
        drawRectButton(popupMinuteIncrementRect, "▲");
        drawRectButton(popupMinuteDecrementRect, "▼");
    }

    popupNeedsRedraw = false;
}

void TimeSettingsScreen::drawPopupSingleValue(const char* title, int value) {
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(popupRect.x + 20, popupRect.y + 25);
    tft->print(title);

    tft->setFont(&fonts::lgfxJapanGothic_16);
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", value);
    int16_t tx = popupValueRect.x + (popupValueRect.w / 2) - (tft->textWidth(buffer) / 2);
    int16_t ty = popupValueRect.y + (popupValueRect.h / 2) - (tft->fontHeight() / 2);
    tft->setCursor(tx, ty);
    tft->print(buffer);
    tft->setFont(nullptr);
}

void TimeSettingsScreen::drawPopupTime() {
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(popupRect.x + 20, popupRect.y + 25);
    tft->print("時刻");

    char buffer[8];
    tft->setFont(&fonts::lgfxJapanGothic_16);

    // Hour value
    snprintf(buffer, sizeof(buffer), "%02d", popupHourValue);
    int16_t hx = popupHourIncrementRect.x + (popupHourIncrementRect.w / 2) - (tft->textWidth(buffer) / 2);
    int16_t hy = popupValueRect.y + (popupValueRect.h / 2) - (tft->fontHeight() / 2);
    tft->setCursor(hx, hy);
    tft->print(buffer);

    // Colon
    tft->setCursor(popupRect.x + popupRect.w / 2 - 8, popupValueRect.y + (popupValueRect.h / 2) - (tft->fontHeight() / 2));
    tft->print(":");

    // Minute value
    snprintf(buffer, sizeof(buffer), "%02d", popupMinuteValue);
    int16_t mx = popupMinuteIncrementRect.x + (popupMinuteIncrementRect.w / 2) - (tft->textWidth(buffer) / 2);
    int16_t my = popupValueRect.y + (popupValueRect.h / 2) - (tft->fontHeight() / 2);
    tft->setCursor(mx, my);
    tft->print(buffer);

    tft->setFont(nullptr);
}

void TimeSettingsScreen::handlePopupTouch(const Event& event) {
    if (!popupActive) {
        return;
    }

    const int32_t x = event.data.touch.x;
    const int32_t y = event.data.touch.y;

    switch (event.type) {
        case EVENT_TOUCH_DOWN:
            if (popupField != TimeField::Time) {
                if (isPointInside(popupIncrementRect, x, y)) {
                    adjustValue(popupValue, popupMinValue, popupMaxValue, 1, popupWrap);
                    popupNeedsRedraw = true;
                    drawPopup();
                } else if (isPointInside(popupDecrementRect, x, y)) {
                    adjustValue(popupValue, popupMinValue, popupMaxValue, -1, popupWrap);
                    popupNeedsRedraw = true;
                    drawPopup();
                }
            } else {
                if (isPointInside(popupHourIncrementRect, x, y)) {
                    adjustValue(popupHourValue, 0, 23, 1, true);
                    popupNeedsRedraw = true;
                    drawPopup();
                } else if (isPointInside(popupHourDecrementRect, x, y)) {
                    adjustValue(popupHourValue, 0, 23, -1, true);
                    popupNeedsRedraw = true;
                    drawPopup();
                } else if (isPointInside(popupMinuteIncrementRect, x, y)) {
                    adjustValue(popupMinuteValue, 0, 59, 1, true);
                    popupNeedsRedraw = true;
                    drawPopup();
                } else if (isPointInside(popupMinuteDecrementRect, x, y)) {
                    adjustValue(popupMinuteValue, 0, 59, -1, true);
                    popupNeedsRedraw = true;
                    drawPopup();
                }
            }
            break;
        case EVENT_TOUCH_UP:
            if (isPointInside(popupOkRect, x, y)) {
                closePopup(true);
            }
            break;
        default:
            break;
    }
}

bool TimeSettingsScreen::isPointInside(const Rect& rect, int32_t x, int32_t y) const {
    return x >= rect.x && x < (rect.x + rect.w) && y >= rect.y && y < (rect.y + rect.h);
}

void TimeSettingsScreen::adjustValue(int& value, int minValue, int maxValue, int delta, bool wrap) {
    value += delta;
    if (value > maxValue) {
        value = wrap ? minValue : maxValue;
    } else if (value < minValue) {
        value = wrap ? maxValue : minValue;
    }
}

void TimeSettingsScreen::syncPopupToField() {
    switch (popupField) {
        case TimeField::Year:
            popupValue = year;
            popupMinValue = 2000;
            popupMaxValue = 2099;
            popupWrap = false;
            break;
        case TimeField::Month:
            popupValue = month;
            popupMinValue = 1;
            popupMaxValue = 12;
            popupWrap = true;
            break;
        case TimeField::Day:
            popupValue = day;
            popupMinValue = 1;
            popupMaxValue = 31;
            popupWrap = true;
            break;
        case TimeField::Time:
            popupHourValue = hour;
            popupMinuteValue = minute;
            break;
        default:
            break;
    }
}

void TimeSettingsScreen::applyPopupToField() {
    switch (popupField) {
        case TimeField::Year:
            year = popupValue;
            break;
        case TimeField::Month:
            month = popupValue;
            if (day > 31) day = 31;
            break;
        case TimeField::Day:
            day = popupValue;
            break;
        case TimeField::Time:
            hour = popupHourValue;
            minute = popupMinuteValue;
            break;
        default:
            break;
    }
}
