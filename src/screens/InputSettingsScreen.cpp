#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "InputSettingsScreen.h"
#include <Arduino.h>
#include "../ui/components/ModernButton.h"
#include "../shared/EventQueue.h"
#include <memory>
#include <vector>
#include <SPI.h>
#include <SD.h>

#ifndef INPUT_SETTINGS_SD_SPI_HOST
#define INPUT_SETTINGS_SD_SPI_HOST VSPI
#endif

#ifndef INPUT_SETTINGS_SD_CS_PIN
#define INPUT_SETTINGS_SD_CS_PIN 5
#endif

#ifndef INPUT_SETTINGS_SD_SCLK_PIN
#define INPUT_SETTINGS_SD_SCLK_PIN 18
#endif

#ifndef INPUT_SETTINGS_SD_MOSI_PIN
#define INPUT_SETTINGS_SD_MOSI_PIN 23
#endif

#ifndef INPUT_SETTINGS_SD_MISO_PIN
#define INPUT_SETTINGS_SD_MISO_PIN 19
#endif

#ifndef INPUT_SETTINGS_SD_FREQ_HZ
#define INPUT_SETTINGS_SD_FREQ_HZ 20000000
#endif

namespace {
constexpr int SDCARD_CS_PIN = INPUT_SETTINGS_SD_CS_PIN;
constexpr int SDCARD_SCLK_PIN = INPUT_SETTINGS_SD_SCLK_PIN;
constexpr int SDCARD_MOSI_PIN = INPUT_SETTINGS_SD_MOSI_PIN;
constexpr int SDCARD_MISO_PIN = INPUT_SETTINGS_SD_MISO_PIN;
constexpr uint32_t SDCARD_SPI_FREQ_HZ = INPUT_SETTINGS_SD_FREQ_HZ;

SPIClass sdSPI(INPUT_SETTINGS_SD_SPI_HOST);
bool sdSpiConfigured = false;

void ensureSdSpiConfigured()
{
    if (sdSpiConfigured) {
        return;
    }
    sdSPI.begin(SDCARD_SCLK_PIN, SDCARD_MISO_PIN, SDCARD_MOSI_PIN, SDCARD_CS_PIN);
    sdSpiConfigured = true;
}
} // namespace

extern EventQueue* g_touchEventQueue;

InputSettingsScreen::InputSettingsScreen(LGFX* display)
    : BaseScreen(display, SCREEN_INPUT_SETTINGS) {}

void InputSettingsScreen::createButtons() {
    buttons.clear();
    auto backBtn = std::unique_ptr<ModernButton>(
        new ModernButton(tft, tft->width() - 70, 10, 60, 30, "戻る")
    );
    ButtonStyle backStyle;
    backStyle.normalColor = tft->color565(96, 125, 139);    // Blue Grey
    backStyle.pressedColor = tft->color565(69, 90, 100);
    backStyle.cornerRadius = 5;
    backStyle.shadowOffset = 2;
    backBtn->setStyle(backStyle);
    backBtn->setOnClick([this]() {
        Event e;
        e.type = EVENT_SCREEN_CHANGE;
        e.data.screenChange.targetScreen = SCREEN_MENU;
        e.data.screenChange.transition = TRANSITION_SLIDE_RIGHT;
        if (g_touchEventQueue) g_touchEventQueue->send(e);
    });
    buttons.push_back(std::move(backBtn));
}

void InputSettingsScreen::checkSDAndCountMp3() {
    sdAvailable = false;
    mp3Count = 0;
    sdErrorMsg.clear();

    ensureSdSpiConfigured();
    // LovyanGFX が利用しているバスを再初期化して衝突を避ける
    if (tft) {
        tft->initBus();
    }

    if (!SD.begin(SDCARD_CS_PIN, sdSPI, SDCARD_SPI_FREQ_HZ)) {
        sdErrorMsg = "SDカードが見つかりません（配線やFAT32フォーマットも確認してください）";
        return;
    }
    File soundDir = SD.open("/sound");
    if (!soundDir || !soundDir.isDirectory()) {
        sdErrorMsg = "/sound フォルダがありません";
        return;
    }
    while (true) {
        File entry = soundDir.openNextFile();
        if (!entry) {
            break;
        }
        String name = entry.name();
        name.toLowerCase();
        if (!entry.isDirectory() && name.endsWith(".mp3")) {
            mp3Count++;
        }
        entry.close();
    }
    soundDir.close();
    sdAvailable = true;
}

void InputSettingsScreen::init() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->setFont(&fonts::lgfxJapanGothic_16);
    tft->setCursor(10, 20);
    tft->println("入力設定");
    // SDカード状況・楽曲数表示は12ptで
    tft->setFont(&fonts::lgfxJapanGothic_12);
    tft->setCursor(10, 60);
    if (sdAvailable) {
        char buf[32];
        sprintf(buf, ".mp3音源: %d個", mp3Count);
        tft->println(buf);
    } else {
        tft->setTextColor(TFT_RED);
        tft->println(sdErrorMsg.c_str());
        tft->setTextColor(TFT_WHITE);
    }
    tft->setFont(nullptr);
    for (auto& button : buttons) {
        button->draw();
    }
}

void InputSettingsScreen::draw() {
    if (needsRedraw) { init(); needsRedraw = false; }
}

void InputSettingsScreen::update() {
}

void InputSettingsScreen::handleEvent(const Event& event) {
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

void InputSettingsScreen::onEnter() {
    checkSDAndCountMp3();
    createButtons();
    needsRedraw = true;
}

void InputSettingsScreen::onExit() {
}
