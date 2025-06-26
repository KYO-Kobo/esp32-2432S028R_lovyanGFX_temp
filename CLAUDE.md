# ESP32-2432S028R Event-Driven Architecture Project

## プロジェクト概要
ESP32-2432S028R（Cheap Yellow Display）を使用したEvent-Driven Architectureの実装プロジェクトです。
AI + 無料ツール品質保証フローを導入し、高品質なC/C++開発を実現します。

## クイックスタート

### 🚀 一発セットアップ
```bash
# 開発環境の完全セットアップ（初回のみ）
make setup
```

このコマンド一つで以下が自動インストールされます：
- **PlatformIO**: ESP32開発環境
- **Claude Code**: AIアシスタント
- **GitHub CLI**: Git操作支援
- **品質チェックツール**: Cppcheck, Clang-Tidy, lcov
- **プロジェクト依存関係**: ライブラリ等

### 📋 コマンド一覧

#### 使用量確認
```bash
ccusage
```

#### 開発環境セットアップ
```bash
make setup                 # 完全セットアップ（推奨）
make setup-brew            # Homebrewインストール (macOS)
make setup-python          # Python環境セットアップ
make setup-platformio      # PlatformIOインストール
make setup-claude          # Claude Codeインストール
make setup-github          # GitHub CLIセットアップ
make setup-quality-tools   # 品質チェックツールインストール
make setup-project         # プロジェクト依存関係インストール
make verify                # インストール状況確認
```

#### デバイス一覧
```bash
pio device list
```

#### ビルド・アップロード
```bash
pio run                    # プロジェクトビルド
pio run --target upload    # ESP32にアップロード
pio device monitor         # シリアルモニター
pio run -e debug          # デバッグビルド
pio run -e release        # リリースビルド
```

#### 品質チェック（AI + 無料ツール品質保証フロー）
```bash
pio run -e quality-check   # 🎯 全品質チェック実行
pio run -e cppcheck        # Cppcheck静的解析
pio run -e clang-tidy      # Clang-Tidy品質チェック
pio run -e coverage        # テストカバレッジ計測
pio test                   # 単体テスト実行
```

#### プロジェクト管理
```bash
make new-project          # 新規プロジェクト作成
make dev                  # VSCodeで開発環境起動
make clean                # 一時ファイル削除
make help                 # ヘルプ表示
```

## 🔧 開発フロー

### 1. 初回セットアップ
```bash
git clone <repository>
cd <project-directory>
make setup
```

### 2. 日常開発
```bash
# コード編集
make dev                   # VSCode起動

# ビルド・テスト
pio run                    # ビルド確認
pio run -e quality-check   # 品質チェック

# デプロイ
pio run --target upload    # ESP32にアップロード
pio device monitor         # 動作確認
```

### 3. コミット前チェック
```bash
pio run -e quality-check   # 必須：品質チェック実行
# ✅ 全チェック通過後にコミット
```

## 📊 品質保証フロー

### AI + 無料ツール3段階品質チェック
1. **Phase 1**: ビルドテスト
2. **Phase 2**: Cppcheck静的解析（MISRA-C準拠）
3. **Phase 3**: Clang-Tidy品質チェック

*注: 単体テストとカバレッジは時間がかかるため、個別実行推奨*

### 品質基準
- **Cppcheck**: 警告0件
- **Clang-Tidy**: 重要度高警告0件

### 個別実行コマンド
```bash
# 高速品質チェック（推奨）
pio run -e quality-check      # 3段階チェック（数秒）

# 詳細チェック（時間かかる）
pio test                      # 単体テスト（30秒〜2分）
pio run -e coverage          # カバレッジ計測（要テスト）
```

## 🎯 プロジェクト構成

### デュアルコア・日本語対応・モジュラーUI構造

```
ESP32-2432S028R/
├── src/
│   ├── main.cpp                    # エントリーポイント
│   ├── core/                       # コア管理
│   │   ├── Core0Manager.cpp/h      # Core 0（表示系）管理
│   │   ├── Core1Manager.cpp/h      # Core 1（I/O・計算系）管理
│   │   ├── TaskManager.cpp/h       # タスク管理
│   │   └── InterCoreComm.cpp/h     # コア間通信
│   ├── display/                    # Core 0専用 - 表示系
│   │   ├── DisplayManager.cpp/h    # ディスプレイ制御
│   │   ├── ScreenManager.cpp/h     # 画面管理
│   │   ├── graphics/               # グラフィック描画
│   │   │   ├── GraphicsEngine.cpp/h    # 描画エンジン
│   │   │   ├── Canvas.cpp/h           # キャンバス
│   │   │   └── ImageRenderer.cpp/h     # 画像描画
│   │   ├── text/                   # 日本語テキスト対応
│   │   │   ├── TextRenderer.cpp/h      # テキスト描画エンジン
│   │   │   ├── FontManager.cpp/h       # フォント管理
│   │   │   ├── JapaneseFonts.cpp/h     # 日本語フォント
│   │   │   ├── StringResource.cpp/h    # 文字列リソース
│   │   │   └── TextLayout.cpp/h        # テキストレイアウト
│   │   ├── ui/                     # UIコンポーネント
│   │   │   ├── base/               # 基底クラス
│   │   │   │   ├── Widget.cpp/h        # ウィジェット基底
│   │   │   │   ├── Container.cpp/h     # コンテナ基底
│   │   │   │   └── Drawable.cpp/h      # 描画可能オブジェクト
│   │   │   ├── components/         # UIパーツ
│   │   │   │   ├── Button.cpp/h        # ボタン
│   │   │   │   ├── Label.cpp/h         # ラベル
│   │   │   │   ├── TextBox.cpp/h       # テキストボックス
│   │   │   │   ├── Slider.cpp/h        # スライダー
│   │   │   │   ├── Checkbox.cpp/h      # チェックボックス
│   │   │   │   ├── ProgressBar.cpp/h   # プログレスバー
│   │   │   │   ├── ImageView.cpp/h     # 画像表示
│   │   │   │   └── Menu.cpp/h          # メニュー
│   │   │   ├── layout/             # レイアウト管理
│   │   │   │   ├── LayoutManager.cpp/h # レイアウト管理
│   │   │   │   ├── GridLayout.cpp/h    # グリッドレイアウト
│   │   │   │   ├── FlexLayout.cpp/h    # フレックスレイアウト
│   │   │   │   └── AbsoluteLayout.cpp/h # 絶対配置
│   │   │   ├── styles/             # スタイル・テーマ
│   │   │   │   ├── Theme.cpp/h         # テーマ管理
│   │   │   │   ├── StyleSheet.cpp/h    # スタイルシート
│   │   │   │   └── ColorPalette.cpp/h  # カラーパレット
│   │   │   └── events/             # UIイベント
│   │   │       ├── UIEventHandler.cpp/h # UIイベント処理
│   │   │       ├── TouchEvent.cpp/h     # タッチイベント
│   │   │       └── KeyEvent.cpp/h       # キーイベント
│   │   ├── screens/                # 画面定義
│   │   │   ├── BaseScreen.cpp/h        # 画面基底クラス
│   │   │   ├── HomeScreen.cpp/h        # ホーム画面
│   │   │   ├── SettingsScreen.cpp/h    # 設定画面
│   │   │   └── CalibrationScreen.cpp/h # キャリブレーション画面
│   │   └── animation/              # アニメーション
│   │       ├── AnimationEngine.cpp/h   # アニメーションエンジン
│   │       ├── Transition.cpp/h        # 画面遷移
│   │       └── Effects.cpp/h           # エフェクト
│   ├── input/                      # Core 1専用 - 入力系
│   │   ├── TouchManager.cpp/h      # タッチ処理
│   │   ├── GestureDetector.cpp/h   # ジェスチャー検出
│   │   ├── TouchCalibration.cpp/h  # キャリブレーション
│   │   └── InputFilter.cpp/h       # 入力フィルタリング
│   ├── communication/              # Core 1専用 - 通信系
│   │   ├── I2CManager.cpp/h        # I2C通信
│   │   ├── BLEManager.cpp/h        # BLE通信
│   │   ├── WiFiManager.cpp/h       # WiFi通信
│   │   └── SerialComm.cpp/h        # シリアル通信
│   ├── processing/                 # Core 1専用 - 計算系
│   │   ├── DataProcessor.cpp/h     # データ処理
│   │   ├── CalculationEngine.cpp/h # 数値計算
│   │   ├── SensorProcessor.cpp/h   # センサーデータ処理
│   │   └── AlgorithmEngine.cpp/h   # アルゴリズム実行
│   ├── shared/                     # 両コア共通
│   │   ├── Events.h                # イベント定義
│   │   ├── EventManager.cpp/h      # イベント管理
│   │   ├── EventQueue.cpp/h        # スレッドセーフキュー
│   │   ├── SharedData.cpp/h        # 共有データ管理
│   │   └── SyncPrimitives.cpp/h    # 同期プリミティブ
│   ├── assets/                     # リソース
│   │   ├── fonts/                  # フォントファイル
│   │   │   ├── JapaneseFont.h      # 日本語フォントデータ
│   │   │   ├── ASCIIFont.h         # ASCII フォント
│   │   │   └── IconFont.h          # アイコンフォント
│   │   ├── images/                 # 画像リソース
│   │   │   ├── icons/              # アイコン
│   │   │   └── backgrounds/        # 背景画像
│   │   └── strings/                # 文字列リソース
│   │       ├── japanese.h          # 日本語文字列
│   │       └── english.h           # 英語文字列
│   ├── config/
│   │   ├── Config.h                # システム設定
│   │   ├── Pins.h                  # ピン定義
│   │   ├── TaskConfig.h            # タスク設定
│   │   ├── DisplayConfig.h         # ディスプレイ設定
│   │   ├── FontConfig.h            # フォント設定
│   │   └── UIConfig.h              # UI設定
│   └── utils/
│       ├── Logger.cpp/h            # ログシステム
│       ├── MemoryManager.cpp/h     # メモリ管理
│       ├── PerformanceMonitor.cpp/h # パフォーマンス監視
│       └── StringUtils.cpp/h       # 文字列ユーティリティ
├── test/                           # 単体テスト
├── scripts/                        # 品質チェックスクリプト
├── ai-rules/                       # AI開発ルール
├── platformio.ini                  # PlatformIO設定
└── Makefile                        # セットアップ自動化
```

## 🏗️ アーキテクチャ設計

### ESP32デュアルコア活用戦略

ESP32の2つのコア（Core 0: PRO_CPU、Core 1: APP_CPU）を効率的に活用し、リアルタイム性とパフォーマンスを最大化します。

#### Core 0 (PRO_CPU) - 表示系専用
```cpp
// Core 0 FreeRTOS Tasks (優先度順)
DisplayTask         (Priority: 3, Stack: 8KB)  - 画面描画・更新
UIUpdateTask        (Priority: 2, Stack: 4KB)  - UI状態管理
AnimationTask       (Priority: 1, Stack: 4KB)  - アニメーション処理
ScreenTransition    (Priority: 1, Stack: 2KB)  - 画面遷移
```

**担当処理:**
- LovyanGFX による高速画面描画
- UIコンポーネントのレンダリング
- アニメーション・エフェクト処理
- 画面遷移管理

#### Core 1 (APP_CPU) - I/O・計算系専用
```cpp
// Core 1 FreeRTOS Tasks (優先度順)
TouchInputTask      (Priority: 4, Stack: 4KB)  - タッチ入力処理
I2CCommTask         (Priority: 3, Stack: 3KB)  - I2C通信
BLECommTask         (Priority: 2, Stack: 8KB)  - BLE通信
DataProcessingTask  (Priority: 1, Stack: 6KB)  - データ計算・処理
```

**担当処理:**
- XPT2046タッチパネル制御
- I2C/SPI/UART通信処理
- BLE/WiFi通信管理
- センサーデータ処理・数値計算

### コア間通信アーキテクチャ

#### 1. 高速データ転送 - FreeRTOS Queue
```cpp
// UI更新データ転送用
QueueHandle_t ui_update_queue;      // Core 1 → Core 0
QueueHandle_t touch_event_queue;    // Core 1 → Core 0
QueueHandle_t sensor_data_queue;    // Core 1 → Core 0
```

#### 2. 状態同期 - Event Groups
```cpp
// システム状態フラグ
EventGroupHandle_t system_events;
#define DISPLAY_READY_BIT    (1 << 0)
#define TOUCH_READY_BIT      (1 << 1)
#define BLE_CONNECTED_BIT    (1 << 2)
#define CALIBRATION_MODE_BIT (1 << 3)
```

#### 3. 排他制御 - Mutex/Semaphore
```cpp
// 共有リソース保護
SemaphoreHandle_t display_mutex;    // 画面描画制御
SemaphoreHandle_t i2c_mutex;       // I2C バス制御
SemaphoreHandle_t config_mutex;    // 設定データ保護
```

#### 4. 大容量データ共有 - Shared Memory
```cpp
// メモリ効率的な共有データ構造
typedef struct {
    uint8_t touch_data[256];        // タッチデータバッファ
    uint8_t sensor_data[512];       // センサーデータバッファ
    uint8_t display_buffer[1024];   // 表示バッファ
} SharedDataBuffer_t;
```

### Event-Driven Architecture

#### イベントフロー設計
```
[タッチ入力] → [Core 1: TouchManager] → [EventQueue] → [Core 0: UIManager] → [画面更新]
     ↓
[ジェスチャー検出] → [アプリロジック] → [データ処理] → [結果表示]
     ↓
[I2C/BLE通信] → [バックグラウンド処理] → [状態更新] → [UI反映]
```

#### パフォーマンス最適化
- **並列処理**: 画面描画と通信・計算を同時実行
- **優先度制御**: リアルタイム性が重要なタスクを高優先度に設定
- **メモリ管理**: 各コアに適切なスタックサイズを割り当て
- **割り込み最小化**: ポーリングベースで安定動作

## 🌐 日本語対応システム

### UTF-8ベース多言語アーキテクチャ

ESP32の限られたメモリを効率的に使用しながら、美しい日本語表示を実現します。

#### フォント管理システム
```cpp
// FontManager.h - メモリ効率的なフォント管理
class FontManager {
private:
    // フォントキャッシュ（よく使う文字を優先保持）
    LRUCache<uint16_t, GlyphData> glyph_cache;
    
public:
    // 日本語フォント種別
    enum FontType {
        HIRAGANA,           // ひらがな（3KB）
        KATAKANA,           // カタカナ（3KB）
        KANJI_BASIC,        // 基本漢字（JIS第1水準: 50KB）
        KANJI_EXTENDED,     // 拡張漢字（JIS第2水準: 100KB）
        ASCII,              // 英数字（2KB）
        ICONS              // アイコンフォント（5KB）
    };
    
    GlyphData* getGlyph(uint16_t unicode, FontType type, uint8_t size);
    void preloadCommonKanji();  // よく使う漢字を事前ロード
};
```

#### 文字列リソース管理
```cpp
// StringResource.h - 多言語文字列管理
namespace Strings {
    enum Language { JP, EN };
    
    // 文字列ID定義
    enum StringID {
        TITLE_HOME = 0,
        BUTTON_SETTINGS,
        LABEL_TEMPERATURE,
        MESSAGE_CALIBRATION,
        // ... 最大256個
    };
    
    const char* getString(StringID id, Language lang = JP);
}

// japanese.h - 日本語文字列定義
const char* const japanese_strings[] = {
    "ホーム",           // TITLE_HOME
    "設定",            // BUTTON_SETTINGS  
    "温度",            // LABEL_TEMPERATURE
    "キャリブレーション中...", // MESSAGE_CALIBRATION
};
```

#### テキストレンダリングエンジン
```cpp
// TextRenderer.h - 高性能日本語描画
class TextRenderer {
private:
    FontManager* font_manager;
    Canvas* canvas;
    
public:
    // 日本語レイアウトオプション
    struct TextStyle {
        uint8_t font_size = 16;
        uint16_t color = 0xFFFF;
        bool bold = false;
        bool enable_kerning = true;     // カーニング調整
        bool enable_ligature = false;   // 合字処理
    };
    
    // 高性能テキスト描画
    void drawText(const char* utf8_text, int x, int y, TextStyle style);
    void drawTextCentered(const char* utf8_text, Rect bounds, TextStyle style);
    
    // レイアウト計算
    Size measureText(const char* utf8_text, TextStyle style);
    std::vector<LineInfo> layoutText(const char* utf8_text, int max_width);
};
```

#### メモリ最適化戦略
```cpp
// 階層的フォントロード戦略
class HierarchicalFontLoader {
public:
    // レベル1: 最小限（基本UI用）
    void loadMinimalSet() {
        loadFont(ASCII);
        loadFont(HIRAGANA);
        loadCommonKanji(50);  // 最頻出50字
    }
    
    // レベル2: 標準（一般アプリ用）
    void loadStandardSet() {
        loadMinimalSet();
        loadFont(KATAKANA);
        loadCommonKanji(300); // 常用漢字
    }
    
    // レベル3: 完全（フル機能用）
    void loadFullSet() {
        loadStandardSet();
        loadFont(KANJI_BASIC);
        loadFont(ICONS);
    }
};
```

### パフォーマンス最適化

#### 1. 文字キャッシュ戦略
- **LRUキャッシュ**: 最近使用した文字グリフを保持
- **事前ロード**: アプリ起動時に頻出文字を先読み
- **部分ロード**: 必要な文字セットのみメモリ展開

#### 2. 描画最適化
- **バッチ描画**: 複数文字をまとめて描画処理
- **ダーティ領域**: 変更部分のみ再描画
- **ハードウェア加速**: ESP32のDMA機能活用

#### 3. メモリ管理
```cpp
// メモリ使用量目安
Font Memory Usage:
├── ASCII (必須)           : 2KB
├── ひらがな (必須)         : 3KB  
├── カタカナ (推奨)         : 3KB
├── 基本漢字 (300字)       : 15KB
├── 拡張漢字 (1000字)      : 50KB
└── グリフキャッシュ        : 10KB
                     合計: 83KB (全体の約25%)
```

## 🎨 UIコンポーネントシステム

### モジュラー設計による再利用可能UI

オブジェクト指向設計とコンポーネントベースアーキテクチャにより、保守性と拡張性の高いUIシステムを構築します。

#### 基底クラス階層
```cpp
// Widget.h - 全UIコンポーネントの基底クラス
class Widget {
protected:
    Rect bounds;                    // 位置・サイズ
    bool visible = true;            // 表示状態
    bool enabled = true;            // 有効状態
    Widget* parent = nullptr;       // 親ウィジェット
    
public:
    // 基本操作
    virtual void draw(Canvas* canvas) = 0;
    virtual bool handleTouch(TouchEvent& event) { return false; }
    virtual void update(uint32_t delta_ms) {}
    
    // レイアウト
    virtual Size getPreferredSize() = 0;
    virtual void setBounds(const Rect& bounds) { this->bounds = bounds; }
    
    // イベント処理
    std::function<void(Widget*)> onClick;
    std::function<void(Widget*, bool)> onStateChanged;
};

// Container.h - 子ウィジェットを持つコンテナ
class Container : public Widget {
protected:
    std::vector<std::unique_ptr<Widget>> children;
    LayoutManager* layout_manager = nullptr;
    
public:
    void addChild(std::unique_ptr<Widget> child);
    void removeChild(Widget* child);
    void setLayout(std::unique_ptr<LayoutManager> layout);
    
    void draw(Canvas* canvas) override;
    bool handleTouch(TouchEvent& event) override;
};
```

#### 基本UIコンポーネント
```cpp
// Button.h - 高機能ボタンコンポーネント
class Button : public Widget {
private:
    String text;
    ButtonState state = NORMAL;     // NORMAL, PRESSED, DISABLED
    ButtonStyle style;
    
public:
    enum ButtonState { NORMAL, PRESSED, DISABLED };
    
    struct ButtonStyle {
        Color normal_color = Color::BLUE;
        Color pressed_color = Color::DARK_BLUE;
        Color disabled_color = Color::GRAY;
        Color text_color = Color::WHITE;
        uint8_t font_size = 16;
        uint8_t border_radius = 8;
        bool enable_animation = true;
    };
    
    Button(const String& text, ButtonStyle style = ButtonStyle{});
    
    void draw(Canvas* canvas) override;
    bool handleTouch(TouchEvent& event) override;
    Size getPreferredSize() override;
};

// Label.h - 日本語対応ラベル
class Label : public Widget {
private:
    String text;
    TextStyle text_style;
    TextAlign alignment = LEFT;
    
public:
    enum TextAlign { LEFT, CENTER, RIGHT };
    
    Label(const String& text, TextStyle style = TextStyle{});
    
    void setText(const String& text);
    void setTextStyle(const TextStyle& style);
    void setAlignment(TextAlign align) { alignment = align; }
    
    void draw(Canvas* canvas) override;
    Size getPreferredSize() override;
};

// Slider.h - 値調整用スライダー
class Slider : public Widget {
private:
    float value = 0.0f;             // 現在値 (0.0-1.0)
    float min_value = 0.0f;
    float max_value = 100.0f;
    SliderStyle style;
    
public:
    struct SliderStyle {
        Color track_color = Color::LIGHT_GRAY;
        Color fill_color = Color::BLUE;
        Color thumb_color = Color::WHITE;
        uint8_t track_height = 4;
        uint8_t thumb_radius = 12;
    };
    
    Slider(float min_val, float max_val, float initial_val = 0.0f);
    
    void setValue(float val);
    float getValue() const { return value; }
    
    void draw(Canvas* canvas) override;
    bool handleTouch(TouchEvent& event) override;
    Size getPreferredSize() override;
    
    std::function<void(float)> onValueChanged;
};
```

#### レイアウトシステム
```cpp
// LayoutManager.h - 自動レイアウト基底
class LayoutManager {
public:
    virtual void layoutChildren(Container* container) = 0;
    virtual Size calculateMinimumSize(Container* container) = 0;
};

// GridLayout.h - グリッドレイアウト
class GridLayout : public LayoutManager {
private:
    uint8_t columns;
    uint8_t rows;
    uint8_t spacing = 8;
    
public:
    GridLayout(uint8_t cols, uint8_t rows, uint8_t spacing = 8);
    
    void layoutChildren(Container* container) override;
    Size calculateMinimumSize(Container* container) override;
};

// FlexLayout.h - フレックスレイアウト
class FlexLayout : public LayoutManager {
public:
    enum Direction { HORIZONTAL, VERTICAL };
    enum Justify { START, CENTER, END, SPACE_BETWEEN };
    enum Align { FLEX_START, FLEX_CENTER, FLEX_END, STRETCH };
    
private:
    Direction direction = HORIZONTAL;
    Justify justify_content = START;
    Align align_items = FLEX_START;
    uint8_t gap = 0;
    
public:
    FlexLayout(Direction dir = HORIZONTAL);
    
    void setDirection(Direction dir) { direction = dir; }
    void setJustifyContent(Justify justify) { justify_content = justify; }
    void setAlignItems(Align align) { align_items = align; }
    void setGap(uint8_t gap_px) { gap = gap_px; }
    
    void layoutChildren(Container* container) override;
    Size calculateMinimumSize(Container* container) override;
};
```

#### テーマ・スタイルシステム
```cpp
// Theme.h - 統一されたデザインシステム
class Theme {
public:
    // カラーパレット
    struct Colors {
        Color primary = Color::BLUE;
        Color secondary = Color::GRAY;
        Color accent = Color::GREEN;
        Color background = Color::BLACK;
        Color surface = Color::DARK_GRAY;
        Color text_primary = Color::WHITE;
        Color text_secondary = Color::LIGHT_GRAY;
        Color error = Color::RED;
        Color warning = Color::ORANGE;
        Color success = Color::GREEN;
    } colors;
    
    // タイポグラフィ
    struct Typography {
        uint8_t heading_size = 24;
        uint8_t body_size = 16;
        uint8_t caption_size = 12;
        String font_family = "NotoSansCJK";
    } typography;
    
    // スペーシング
    struct Spacing {
        uint8_t xs = 4;
        uint8_t sm = 8;
        uint8_t md = 16;
        uint8_t lg = 24;
        uint8_t xl = 32;
    } spacing;
    
    // 各コンポーネントのデフォルトスタイル
    ButtonStyle getButtonStyle(ButtonVariant variant = PRIMARY);
    TextStyle getTextStyle(TextVariant variant = BODY);
    SliderStyle getSliderStyle();
};

// StyleSheet.h - CSS風スタイル定義
class StyleSheet {
private:
    std::map<String, Style> styles;
    
public:
    struct Style {
        Color background_color;
        Color border_color;
        uint8_t border_width = 0;
        uint8_t border_radius = 0;
        Padding padding;
        Margin margin;
    };
    
    void defineStyle(const String& name, const Style& style);
    Style getStyle(const String& name) const;
    void applyStyle(Widget* widget, const String& style_name);
};
```

### コンポーネント使用例
```cpp
// 実際の画面構築例
void createSettingsScreen() {
    auto container = std::make_unique<Container>();
    container->setLayout(std::make_unique<FlexLayout>(FlexLayout::VERTICAL));
    
    // タイトル
    auto title = std::make_unique<Label>("設定", theme.getTextStyle(HEADING));
    title->setAlignment(Label::CENTER);
    container->addChild(std::move(title));
    
    // 明度調整スライダー
    auto brightness_slider = std::make_unique<Slider>(0, 100, 80);
    brightness_slider->onValueChanged = [](float value) {
        adjustDisplayBrightness(value);
    };
    container->addChild(std::move(brightness_slider));
    
    // キャリブレーションボタン
    auto calib_button = std::make_unique<Button>("キャリブレーション");
    calib_button->onClick = [](Widget*) {
        startTouchCalibration();
    };
    container->addChild(std::move(calib_button));
    
    screen_manager->setCurrentScreen(std::move(container));
}
```

## ⚡ デュアルコア最適化

### FreeRTOSタスク設計とパフォーマンスチューニング

ESP32の2つのコアを最大限活用し、60FPSの滑らかなUI描画と並行データ処理を実現します。

#### Core 0専用タスク設計
```cpp
// Core0Manager.cpp - 表示系タスク管理
class Core0Manager {
private:
    TaskHandle_t display_task_handle;
    TaskHandle_t ui_update_task_handle;
    TaskHandle_t animation_task_handle;
    
public:
    void initializeTasks() {
        // 最高優先度：リアルタイム描画
        xTaskCreatePinnedToCore(
            displayTask,            // タスク関数
            "DisplayTask",          // タスク名
            8192,                   // スタックサイズ
            this,                   // パラメータ
            3,                      // 優先度（最高）
            &display_task_handle,   // ハンドル
            0                       // Core 0に固定
        );
        
        // 高優先度：UI状態更新
        xTaskCreatePinnedToCore(
            uiUpdateTask,
            "UIUpdateTask",
            4096,
            this,
            2,
            &ui_update_task_handle,
            0
        );
        
        // 中優先度：アニメーション
        xTaskCreatePinnedToCore(
            animationTask,
            "AnimationTask",
            4096,
            this,
            1,
            &animation_task_handle,
            0
        );
    }
    
private:
    // 60FPS固定描画タスク（16.67ms周期）
    static void displayTask(void* parameter) {
        TickType_t last_wake_time = xTaskGetTickCount();
        const TickType_t frame_delay = pdMS_TO_TICKS(16); // ~60FPS
        
        while (true) {
            // 画面描画処理
            renderCurrentScreen();
            
            // 正確な60FPS維持
            vTaskDelayUntil(&last_wake_time, frame_delay);
        }
    }
    
    // UI状態更新タスク（30FPS）
    static void uiUpdateTask(void* parameter) {
        while (true) {
            updateUIComponents();
            vTaskDelay(pdMS_TO_TICKS(33)); // 30FPS
        }
    }
    
    // アニメーション処理タスク（120FPS - 補間用）
    static void animationTask(void* parameter) {
        while (true) {
            updateAnimations();
            vTaskDelay(pdMS_TO_TICKS(8)); // 120FPS
        }
    }
};
```

#### Core 1専用タスク設計
```cpp
// Core1Manager.cpp - I/O・計算系タスク管理
class Core1Manager {
private:
    TaskHandle_t touch_task_handle;
    TaskHandle_t i2c_task_handle;
    TaskHandle_t ble_task_handle;
    TaskHandle_t data_task_handle;
    
public:
    void initializeTasks() {
        // 最高優先度：タッチ入力（リアルタイム性重要）
        xTaskCreatePinnedToCore(
            touchInputTask,
            "TouchTask",
            4096,
            this,
            4,                      // 最高優先度
            &touch_task_handle,
            1                       // Core 1に固定
        );
        
        // 高優先度：I2C通信
        xTaskCreatePinnedToCore(
            i2cCommTask,
            "I2CTask",
            3072,
            this,
            3,
            &i2c_task_handle,
            1
        );
        
        // 中優先度：BLE通信
        xTaskCreatePinnedToCore(
            bleCommTask,
            "BLETask",
            8192,                   // BLE用大きなスタック
            this,
            2,
            &ble_task_handle,
            1
        );
        
        // 低優先度：データ処理
        xTaskCreatePinnedToCore(
            dataProcessingTask,
            "DataTask",
            6144,
            this,
            1,
            &data_task_handle,
            1
        );
    }
    
private:
    // 高速タッチ入力処理（1000Hz サンプリング）
    static void touchInputTask(void* parameter) {
        while (true) {
            processTouchInput();
            vTaskDelay(pdMS_TO_TICKS(1)); // 1ms = 1000Hz
        }
    }
    
    // I2C通信タスク（100Hz）
    static void i2cCommTask(void* parameter) {
        while (true) {
            processI2CDevices();
            vTaskDelay(pdMS_TO_TICKS(10)); // 10ms = 100Hz
        }
    }
    
    // BLE通信タスク（イベント駆動）
    static void bleCommTask(void* parameter) {
        while (true) {
            processBLEEvents();
            vTaskDelay(pdMS_TO_TICKS(50)); // 20Hz
        }
    }
    
    // データ処理タスク（バックグラウンド）
    static void dataProcessingTask(void* parameter) {
        while (true) {
            processBackgroundData();
            vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz
        }
    }
};
```

#### 高速コア間通信
```cpp
// InterCoreComm.h - 最適化されたコア間通信
class InterCoreComm {
private:
    // 高速リングバッファ（ロックフリー）
    struct FastRingBuffer {
        volatile uint32_t write_index;
        volatile uint32_t read_index;
        uint8_t buffer[4096];
        uint32_t mask = 4095;  // 2^n - 1
        
        bool push(const void* data, size_t size) {
            uint32_t next_write = (write_index + size) & mask;
            if (next_write == read_index) return false; // バッファ満杯
            
            memcpy((void*)(buffer + write_index), data, size);
            write_index = next_write;
            return true;
        }
        
        bool pop(void* data, size_t size) {
            if (read_index == write_index) return false; // バッファ空
            
            memcpy(data, (void*)(buffer + read_index), size);
            read_index = (read_index + size) & mask;
            return true;
        }
    };
    
    FastRingBuffer core1_to_core0_buffer;
    FastRingBuffer core0_to_core1_buffer;
    
public:
    // Core 1 → Core 0 (UIイベント送信)
    bool sendUIEvent(const UIEvent& event) {
        return core1_to_core0_buffer.push(&event, sizeof(UIEvent));
    }
    
    // Core 0 → Core 1 (設定変更送信)
    bool sendConfigUpdate(const ConfigUpdate& update) {
        return core0_to_core1_buffer.push(&update, sizeof(ConfigUpdate));
    }
    
    // イベント処理
    void processCore1Events() {
        UIEvent event;
        while (core1_to_core0_buffer.pop(&event, sizeof(UIEvent))) {
            handleUIEvent(event);
        }
    }
};
```

#### メモリ最適化
```cpp
// MemoryManager.h - 効率的メモリ管理
class MemoryManager {
private:
    // コア別メモリプール
    struct CoreMemoryPool {
        uint8_t* heap_start;
        size_t heap_size;
        size_t allocated_size;
    };
    
    CoreMemoryPool core0_pool;  // 表示系専用
    CoreMemoryPool core1_pool;  // I/O・計算系専用
    
public:
    void initializeMemoryPools() {
        // Core 0: 表示系（フレームバッファ重点）
        core0_pool.heap_size = 150 * 1024;  // 150KB
        core0_pool.heap_start = (uint8_t*)heap_caps_malloc(
            core0_pool.heap_size, 
            MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL
        );
        
        // Core 1: I/O・計算系
        core1_pool.heap_size = 100 * 1024;  // 100KB
        core1_pool.heap_start = (uint8_t*)heap_caps_malloc(
            core1_pool.heap_size,
            MALLOC_CAP_INTERNAL
        );
    }
    
    // PSRAM活用戦略
    void optimizePSRAMUsage() {
        // 大容量バッファはPSRAMに配置
        large_image_buffer = (uint8_t*)heap_caps_malloc(
            1024 * 1024,  // 1MB
            MALLOC_CAP_SPIRAM
        );
        
        // 頻繁アクセスデータは内蔵RAMに配置
        touch_sample_buffer = (uint16_t*)heap_caps_malloc(
            1024 * sizeof(uint16_t),
            MALLOC_CAP_INTERNAL
        );
    }
};
```

### パフォーマンス監視
```cpp
// PerformanceMonitor.h - リアルタイム性能監視
class PerformanceMonitor {
private:
    struct TaskStats {
        uint32_t execution_time;    // 実行時間(μs)
        uint32_t max_execution_time;
        uint32_t frame_drops;       // フレーム落ち回数
        float cpu_usage;            // CPU使用率
    };
    
    std::map<String, TaskStats> core0_stats;
    std::map<String, TaskStats> core1_stats;
    
public:
    void logTaskPerformance(const String& task_name, uint32_t execution_time) {
        // パフォーマンス閾値チェック
        if (execution_time > WARNING_THRESHOLD_US) {
            Serial.printf("WARNING: %s took %dμs\n", task_name.c_str(), execution_time);
        }
        
        // 統計更新
        auto& stats = getTaskStats(task_name);
        stats.execution_time = execution_time;
        stats.max_execution_time = max(stats.max_execution_time, execution_time);
    }
    
    void printPerformanceReport() {
        Serial.println("=== Performance Report ===");
        Serial.printf("Core 0 Usage: %.1f%%\n", getCoreUsage(0));
        Serial.printf("Core 1 Usage: %.1f%%\n", getCoreUsage(1));
        Serial.printf("Frame Rate: %.1f FPS\n", getCurrentFPS());
        Serial.printf("Free Heap: %d bytes\n", esp_get_free_heap_size());
    }
};
```

## 📱 ハードウェア仕様

- **ディスプレイ**: 2.4インチ TFT LCD（ST7789, 320x240）
- **タッチ**: 抵抗膜式タッチパネル（XPT2046）
- **MCU**: ESP32-WROOM-32
- **接続**: USB-C, microSD

## 👥 開発ガイドライン

### 新機能開発フロー

#### 1. UIコンポーネント作成
```cpp
// 新しいUIコンポーネントのテンプレート
class CustomWidget : public Widget {
private:
    // 状態管理
    WidgetState state;
    CustomStyle style;
    
public:
    CustomWidget(const CustomStyle& style = CustomStyle{});
    
    // 必須実装メソッド
    void draw(Canvas* canvas) override;
    bool handleTouch(TouchEvent& event) override;
    Size getPreferredSize() override;
    
    // カスタムメソッド
    void setCustomProperty(const PropertyType& value);
    PropertyType getCustomProperty() const;
    
    // イベントハンドラ
    std::function<void(CustomWidget*, EventData)> onCustomEvent;
};
```

#### 2. 日本語文字列の追加
```cpp
// 1. strings/japanese.h に文字列追加
const char* const japanese_strings[] = {
    // ... 既存文字列
    "新しい機能",         // NEW_FEATURE_TITLE
    "設定が保存されました",  // SETTINGS_SAVED_MESSAGE
};

// 2. StringResource.h のenumに追加
enum StringID {
    // ... 既存ID
    NEW_FEATURE_TITLE,
    SETTINGS_SAVED_MESSAGE,
};

// 3. 使用例
auto label = std::make_unique<Label>(
    Strings::getString(Strings::NEW_FEATURE_TITLE),
    theme.getTextStyle(HEADING)
);
```

#### 3. Core間通信の実装
```cpp
// 1. イベント定義 (shared/Events.h)
struct CustomDataEvent {
    EventType type = CUSTOM_DATA;
    uint32_t timestamp;
    float sensor_value;
    bool is_valid;
};

// 2. Core 1での送信 (input/または communication/)
void sendCustomData(float value) {
    CustomDataEvent event;
    event.timestamp = millis();
    event.sensor_value = value;
    event.is_valid = true;
    
    inter_core_comm.sendUIEvent(event);
}

// 3. Core 0での受信 (display/)
void handleCustomDataEvent(const CustomDataEvent& event) {
    if (event.is_valid) {
        updateDisplayWithValue(event.sensor_value);
    }
}
```

### ベストプラクティス

#### パフォーマンス
```cpp
// ✅ 良い例：効率的な描画
void draw(Canvas* canvas) override {
    // 変更チェック
    if (!isDirty()) return;
    
    // 必要な領域のみ描画
    canvas->setClipRect(bounds);
    drawBackground(canvas);
    drawContent(canvas);
    
    setDirty(false);
}

// ❌ 悪い例：毎回全体描画
void draw(Canvas* canvas) override {
    canvas->clear();  // 画面全体をクリア（重い）
    drawEverything(canvas);  // 毎回全描画
}
```

#### メモリ管理
```cpp
// ✅ 良い例：RAII とスマートポインタ
auto widget = std::make_unique<Button>("ボタン");
container->addChild(std::move(widget));

// ❌ 悪い例：生ポインタとメモリリーク
Button* widget = new Button("ボタン");
container->addChild(widget);  // deleteし忘れの可能性
```

#### タスク設計
```cpp
// ✅ 良い例：適切な優先度とディレイ
void backgroundTask(void* parameter) {
    while (true) {
        processData();
        vTaskDelay(pdMS_TO_TICKS(100));  // 他タスクに制御を譲る
    }
}

// ❌ 悪い例：busy wait
void backgroundTask(void* parameter) {
    while (true) {
        processData();
        // ディレイなし - CPUを占有
    }
}
```

### デバッグ・テスト

#### ログ出力
```cpp
// Logger.h の使用例
Logger::info("UI", "ボタンがクリックされました");
Logger::warning("Touch", "キャリブレーション値が範囲外: %d", raw_value);
Logger::error("Memory", "メモリ不足: %d bytes required", required_size);

// パフォーマンス測定
PERFORMANCE_MEASURE_BEGIN("widget_draw");
widget->draw(canvas);
PERFORMANCE_MEASURE_END("widget_draw");
```

#### 単体テスト
```cpp
// test/test_ui_components.cpp
void test_button_click() {
    Button button("テスト");
    bool clicked = false;
    
    button.onClick = [&clicked](Widget*) {
        clicked = true;
    };
    
    TouchEvent event;
    event.type = TOUCH_DOWN;
    event.x = 10;
    event.y = 10;
    
    button.handleTouch(event);
    
    TEST_ASSERT_TRUE(clicked);
}
```

### トラブルシューティング

#### よくある問題と解決法

1. **文字化け**
   ```cpp
   // 原因：フォントが読み込まれていない
   // 解決：FontManagerの初期化確認
   font_manager->loadFont(FontManager::HIRAGANA);
   ```

2. **タッチが効かない**
   ```cpp
   // 原因：座標変換の問題
   // 解決：キャリブレーション実行
   touch_calibration->startCalibration();
   ```

3. **フレーム落ち**
   ```cpp
   // 原因：描画処理が重い
   // 解決：描画範囲の最適化
   if (widget->isDirty()) {
       widget->draw(canvas);
   }
   ```

4. **メモリ不足**
   ```cpp
   // 原因：メモリフラグメンテーション
   // 解決：適切なヒープ使用
   void* ptr = heap_caps_malloc(size, MALLOC_CAP_INTERNAL);
   ```

## 🔗 参考リンク

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32-2432S028R Hardware Guide](https://github.com/rzeldent/platformio-espressif32-sunton)
- [FreeRTOS Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html)
- [ESP32 Memory Management](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/mem_alloc.html)
- [Claude Code](https://claude.ai/code)

---

## Claude Code Configuration

### YOU MUST: 
- 全てのTODO完了またはユーザー のアクションが必要な際は最後に一度だけ `say "タスク完了"` コマンドを実行して音声通知する
- 回答は日本語で行ってください

### 関数・エンドポイント作成時の注意事項
- 命名規則などを@ai-rules/API_FUNCTION_NAMING.mdにまとめています
- 関数やエンドポイントの作成時には必ず確認し、内容に従って実装を行ってください。

### 修正の際の注意点
- 修正を行う際には必ず以下のことに順守してください
  - 該当修正によって他の処理に問題がないか慎重に確認を行って作業を行ってください。
  - 他の動作に関しても修正が必要な場合は既存の期待値の動作が正常に起動するように修正してください。

### コミット前に確認すること（必ず実施）
- コミット前には必ず動作確認を行って動作が問題ないかを確認してください
  - 動作確認中にエラーが発見された際はタスクを更新してください
  - コミットする際はエラーがない状態で行ってください
- **必須**: `pio run -e quality-check` を実行して全品質チェックを通過させること