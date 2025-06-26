# API関数・命名規則ガイドライン

## 基本命名規則

### クラス名
- **PascalCase**を使用
- 意味のある名前を付ける
- 例：`EventManager`, `ScreenManager`, `GestureDetector`

### 関数名・変数名
- **snake_case**を使用
- 動詞で始める（関数の場合）
- 例：`get_current_screen()`, `handle_swipe()`, `current_x`

### 定数
- **UPPER_SNAKE_CASE**を使用
- 意味を明確にする
- 例：`MAX_HANDLERS`, `SCREEN_WIDTH`, `COLOR_PRIMARY`

### プライベートメンバー
- 先頭に `_` を付ける
- 例：`_instance`, `_touch_active`, `_event_queue`

## ファイル命名規則

### ヘッダーファイル
- **PascalCase**でクラス名と同じ
- 拡張子：`.h`
- 例：`EventManager.h`, `ScreenManager.h`

### ソースファイル
- **PascalCase**でクラス名と同じ
- 拡張子：`.cpp`
- 例：`EventManager.cpp`, `ScreenManager.cpp`

### 複数クラスを含むファイル
- 機能グループ名を使用
- 例：`Screens.h`, `Screens.cpp`

## 関数命名パターン

### アクセサー関数
```cpp
// Getter
int get_value() const;
ScreenID get_screen_id() const;

// Setter
void set_value(int value);
void set_active(bool active);

// Boolean getter
bool is_active() const;
bool has_error() const;
```

### イベント処理関数
```cpp
// ハンドラー
void handle_touch_event(const TouchEvent& event);
void on_screen_enter();
void on_swipe_detected(SwipeDirection direction);

// 登録・解除
bool register_handler(EventType type, EventHandler handler);
void unregister_handler(EventType type);
```

### 初期化・終了処理
```cpp
void initialize(TFT_eSPI* display);
void shutdown();
void cleanup();
```

## 列挙型命名規則

### 列挙型名
- **PascalCase**を使用
- 例：`EventType`, `SwipeDirection`, `ScreenID`

### 列挙値
- **UPPER_SNAKE_CASE**を使用
- 列挙型名の省略形をプレフィックスとして使用可能
- 例：
```cpp
enum class EventType {
    TOUCH_START,
    TOUCH_END,
    SWIPE,
    SCREEN_TRANSITION
};

enum class SwipeDirection {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};
```

## マクロ・定数定義

### プリプロセッサ定義
- **UPPER_SNAKE_CASE**を使用
- 機能グループで分類
- 例：
```cpp
#define MAX_HANDLERS 10
#define MAX_EVENT_QUEUE 20
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
```

### 色定数
- **COLOR_**プレフィックスを使用
- 例：
```cpp
static const uint16_t COLOR_BACKGROUND = TFT_BLACK;
static const uint16_t COLOR_PRIMARY = TFT_BLUE;
static const uint16_t COLOR_ACCENT = TFT_RED;
```

## 変数命名規則

### メンバー変数
```cpp
class Example {
private:
    int _member_variable;        // プライベート
    bool _is_initialized;        // Boolean
    TFT_eSPI* _display;         // ポインタ
    
public:
    int public_variable;         // パブリック
    static int class_variable;   // クラス変数
};
```

### ローカル変数
```cpp
void function_example() {
    int local_variable = 0;
    bool is_valid = true;
    unsigned long current_time = millis();
    
    // ループ変数
    for (int i = 0; i < count; i++) {
        // 処理
    }
}
```

## 特殊な命名規則

### シングルトン
```cpp
class Manager {
private:
    static Manager* _instance;
    
public:
    static Manager& get_instance();
};
```

### コールバック関数
```cpp
using EventHandler = std::function<void(const Event&)>;
using TouchCallback = std::function<void(int x, int y)>;
```

### 名前空間
- 使用しない（Arduino環境では避ける）
- 代わりにクラス名やプレフィックスで区別

## コメント規則

### 関数コメント
```cpp
/**
 * @brief タッチイベントを処理する
 * @param event 処理するタッチイベント
 * @return 処理が成功した場合true
 */
bool handle_touch_event(const TouchEvent& event);
```

### クラスコメント
```cpp
/**
 * @class EventManager
 * @brief イベントの管理と配信を行うシングルトンクラス
 */
class EventManager {
    // 実装
};
```

## ベストプラクティス

1. **一貫性を保つ** - プロジェクト全体で同じ規則を使用
2. **意味のある名前** - 省略形は最小限に
3. **スコープを明確に** - プライベート、パブリックを適切に分離
4. **型安全性** - enum classを使用
5. **メモリ効率** - Arduinoの制約を考慮した命名

## 禁止事項

- **camelCase**の使用（変数・関数）
- **hungarian notation**の使用
- **単文字変数**（ループ変数以外）
- **予約語の使用**
- **曖昧な略語**の使用

この規則に従うことで、コードの可読性と保守性が向上し、C++とArduino環境での開発効率が向上します。