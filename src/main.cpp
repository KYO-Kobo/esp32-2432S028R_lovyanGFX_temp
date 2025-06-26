#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// Pin definitions for ESP32-3224S028R
#define LCD_CS 15
#define LCD_SCK 14
#define LCD_MOSI 13
#define LCD_MISO -1 // Not used
#define LCD_DC 2
#define LCD_RST 4
#define LCD_BL 21 // Changed from 27 to 21

#define TOUCH_CS 33
#define TOUCH_IRQ 32

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_XPT2046 _touch_instance;

public:
    LGFX(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = VSPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = true;
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
            cfg.memory_width = 240;
            cfg.memory_height = 320;
            cfg.panel_width = 240;
            cfg.panel_height = 320;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
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
            cfg.x_min = 3600;     // 左端のRaw Y値
            cfg.x_max = 200;      // 右端のRaw Y値  
            cfg.y_min = 400;      // 下端のRaw X値
            cfg.y_max = 3800;     // 上端のRaw X値
            cfg.pin_int = 36;     // Touch IRQ pin for ESP32-2432S028R
            cfg.bus_shared = false;  // Touch uses separate SPI
            cfg.offset_rotation = 2;  // setRotation(0)に合わせる
            cfg.spi_host = HSPI_HOST;  // Use HSPI for touch
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

LGFX tft;

void setup()
{
    Serial.begin(115200);
    delay(1000);  // シリアルモニター接続待ち
    
    Serial.println("Starting setup...");
    
    tft.begin();
    tft.setRotation(1);  // 縦向きに変更
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Hello World!");
    
    // 画面サイズを表示
    Serial.printf("Screen size: %d x %d\n", tft.width(), tft.height());
    
    // タッチ初期化状態を確認
    lgfx::touch_point_t tp;
    uint8_t touch_state = tft.getTouchRaw(&tp);
    Serial.printf("Touch initialized: %s\n", touch_state ? "Yes" : "No");
    
    // タッチ情報表示エリアを描画
    tft.setTextSize(1);
    tft.setCursor(10, 50);
    tft.println("Touch the screen...");
    tft.setCursor(10, 65);
    tft.printf("Screen: %d x %d", tft.width(), tft.height());
    
    Serial.println("Setup complete");
    Serial.println("Touch detection started");
    
    // タッチキャリブレーション値を表示（デバッグ用）
    tft.setCursor(10, 130);
    tft.println("Touch pins: CLK=25, DIN=32, DO=39, CS=33");
}

void loop()
{
    static uint32_t count = 0;
    
    // デバッグ用：タッチ時の生データを表示
    lgfx::touch_point_t tp;
    if (tft.getTouchRaw(&tp) && tp.size > 0) {
        Serial.printf("Raw touch: x=%d, y=%d, size=%d\n", tp.x, tp.y, tp.size);
        
        // 画面上にRaw値も表示
        tft.fillRect(10, 150, 300, 30, TFT_BLACK);
        tft.setCursor(10, 150);
        tft.setTextSize(1);
        tft.setTextColor(TFT_YELLOW);
        tft.printf("Raw: X=%d, Y=%d", tp.x, tp.y);
    }
    
    // タッチ座標を取得
    int32_t x, y;
    if (tft.getTouch(&x, &y)) {
        // シリアルモニターに座標を出力
        Serial.printf("Touch detected: x=%d, y=%d\n", x, y);
        
        // 画面上の座標表示エリアをクリア
        tft.fillRect(10, 70, 300, 60, TFT_BLACK);
        
        // 画面に座標を表示
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(1);
        tft.setCursor(10, 70);
        tft.printf("Touch Position:");
        
        tft.setCursor(10, 85);
        tft.printf("X: %d", x);
        
        tft.setCursor(10, 100);
        tft.printf("Y: %d", y);
        
        // タッチした位置に小さな円を描画
        tft.fillCircle(x, y, 3, TFT_RED);
        
        // 連続タッチでの描画を防ぐため少し待機
        delay(50);
    }
    
    delay(10);  // CPU負荷軽減
}