#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "core/Core0Manager.h"
#include "core/Core1Manager.h"
#include "shared/EventQueue.h"

// Pin definitions for ESP32-3224S028R
#define LCD_CS 15
#define LCD_SCK 14
#define LCD_MOSI 13
#define LCD_MISO 12 // Not used
#define LCD_DC 2
#define LCD_RST -1
#define LCD_BL 21 // Changed from 27 to 21

#define TOUCH_CS 33
#define TOUCH_IRQ 32

class LGFX_ESP32 : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_XPT2046 _touch_instance;

public:
    LGFX_ESP32(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 80000000;  // 80MHzに増加してちらつきを軽減
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = 1;
            cfg.pin_sclk = LCD_SCK;
            cfg.pin_mosi = LCD_MOSI;
            cfg.pin_miso = LCD_MISO;
            cfg.pin_dc = LCD_DC;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs = LCD_CS;
            cfg.pin_rst = LCD_RST;
            cfg.pin_busy = -1;
            cfg.memory_width = 320;
            cfg.memory_height = 240;
            cfg.panel_width = 320;
            cfg.panel_height = 240;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 5;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = LCD_BL;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        {
            auto cfg = _touch_instance.config();
            // X座標に-10、Y座標に+10のオフセットを適用
            // 画面座標系でのオフセットをraw値に変換
            // X軸: 3600→200 (240ピクセル分), 10ピクセル分 = 142
            // Y軸: 400→3800 (320ピクセル分), 10ピクセル分 = 106
            cfg.x_min = 3650;     // タッチスクリーンから得られる最小のX値(生の値)
            cfg.x_max = 480;      // タッチスクリーンから得られる最大のX値(生の値)
            cfg.y_min = 280 - 180;      // タッチスクリーンから得られる最小のY値(生の値)
            cfg.y_max = 3788;     // タッチスクリーンから得られる最大のY値(生の値)
            cfg.pin_int = 36;     // Touch IRQ pin for ESP32-2432S028R
            cfg.bus_shared = false;  // Touch uses separate SPI
            cfg.offset_rotation = 5;  // setRotation(0)に合わせる
            cfg.spi_host = -1;  // Use HSPI for touch
            cfg.freq = 1000000;
            cfg.pin_sclk = 25;    // Touch CLK pin
            cfg.pin_mosi = 32;    // Touch DIN pin
            cfg.pin_miso = 39;    // Touch DO pin
            cfg.pin_cs = 33;      // Touch CS pin
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }
};

LGFX_ESP32 tft;

// デュアルコアマネージャー
Core0Manager* core0Manager = nullptr;
Core1Manager* core1Manager = nullptr;

void setup()
{
    Serial.begin(115200);
    delay(1000);  // シリアルモニター接続待ち
    
    Serial.println("=== Dual Core Touch Display Starting ===");
    Serial.printf("CPU Frequency: %d MHz\n", getCpuFrequencyMhz());
    Serial.printf("Free Heap: %d bytes\n", esp_get_free_heap_size());
    
    // ディスプレイとタッチパネルの初期化
    tft.begin();
    tft.setRotation(1);  // 横向き
    tft.fillScreen(TFT_BLACK);
    
    // タッチ初期化状態を確認
    lgfx::touch_point_t tp;
    uint8_t touch_state = tft.getTouchRaw(&tp);
    Serial.printf("Touch initialized: %s\n", touch_state ? "Yes" : "No");
    
    // グローバルイベントキューを作成
    g_touchEventQueue = new EventQueue(64);  // 64イベント分のキュー
    Serial.println("Event queue created");
    
    // Core 0 Manager（表示系）を初期化
    core0Manager = new Core0Manager(static_cast<LGFX*>(&tft));
    core0Manager->init();
    
    // Core 1 Manager（タッチ入力系）を初期化
    core1Manager = new Core1Manager(static_cast<LGFX*>(&tft));
    core1Manager->init();
    
    // 各コアでタスクを開始
    Serial.println("Starting dual-core tasks...");
    core0Manager->startTasks();  // Core 0: 表示タスク
    core1Manager->startTasks();  // Core 1: タッチタスク
    
    Serial.println("=== Setup complete ===");
    Serial.printf("Core 0: Display Processing\n");
    Serial.printf("Core 1: Touch Input Processing\n");
}

void loop()
{
    // メインループはタスク管理のみ
    static uint32_t lastStatusUpdate = 0;
    uint32_t now = millis();
    
    // 5秒ごとにステータスを表示
    if (now - lastStatusUpdate > 5000) {
        lastStatusUpdate = now;
        
        Serial.println("\n=== System Status ===");
        Serial.printf("Uptime: %lu seconds\n", now / 1000);
        Serial.printf("Free Heap: %d bytes\n", esp_get_free_heap_size());
        Serial.printf("Event Queue Size: %d\n", g_touchEventQueue ? g_touchEventQueue->getCount() : 0);
        
        // タスク状態を表示
        Serial.printf("Core 0 Stack High Water Mark: %d\n", 
                     uxTaskGetStackHighWaterMark(nullptr));
        Serial.printf("Core 1 Stack High Water Mark: %d\n", 
                     uxTaskGetStackHighWaterMark(nullptr));
    }
    
    // CPU負荷を下げるため待機
    vTaskDelay(pdMS_TO_TICKS(1000));
}