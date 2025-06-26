# 開発ワークフロー - AI + 無料ツール品質保証フロー

## 概要

AIのコード生成と無料ツールを組み合わせた、低コストで品質を担保する開発フローです。
人的コストを最小限に抑えながら、高品質なC/C++コードを継続的に生成・検証します。

## 開発フロー

### 1. コード生成フェーズ
**担当**: AI（Gemini, ChatGPT, Copilot等）

#### 実行内容
- 仕様書・要求仕様をAIに入力
- C/C++コードを自動生成
- 命名規則（[API_FUNCTION_NAMING.md](API_FUNCTION_NAMING.md)）に準拠

#### 成果物
- 生成されたソースコード（.c/.cpp, .h）
- 基本的なコメント・ドキュメント

#### 品質基準
- コンパイルが通ること
- 基本的な機能要件を満たすこと
- 命名規則に準拠していること

---

### 2. 一次チェックフェーズ（Cppcheck）
**担当**: 自動ツール（Cppcheck + MISRA）

#### 実行内容
```bash
# MISRA準拠チェック実行
cppcheck --addon=misra.json --enable=all --error-exitcode=1 src/

# 基本的な静的解析
cppcheck --enable=warning,style,performance,portability src/
```

#### 検出・修正対象
- **メモリリーク**: malloc/freeの不整合
- **バッファオーバーフロー**: 配列境界チェック
- **未初期化変数**: 変数の初期化漏れ
- **MISRA-C違反**: 安全なコーディング規約違反
- **デッドコード**: 到達不可能なコード
- **型変換**: 暗黙的な型変換警告

#### 修正方針
1. Cppcheckの警告を全て解消
2. MISRA-C準拠率90%以上を目標
3. クリティカルエラーは必ず修正

---

### 3. 二次チェックフェーズ（Clang-Tidy）
**担当**: 自動ツール（Clang-Tidy）

#### 実行内容
```bash
# 詳細な静的解析実行
clang-tidy src/*.cpp src/*.c -- -I./src -I./lib

# 特定のチェック項目を指定
clang-tidy -checks='*,-google-*,-fuchsia-*' src/*.cpp
```

#### 検出・修正対象
- **コード品質**: 複雑度、可読性の改善
- **パフォーマンス**: 非効率なアルゴリズム・データ構造
- **モダンC++**: C++11/14/17準拠の改善提案
- **セキュリティ**: 潜在的なセキュリティホール
- **保守性**: リファクタリング提案

#### 修正方針
1. Clang-Tidyの重要度高警告を解消
2. パフォーマンス関連の警告を優先修正
3. 可読性・保守性の改善を適用

---

### 4. ロジック検証フェーズ（単体テスト）
**担当**: 人間（エンジニア）

#### 実行内容
- Unityテストフレームワークを使用
- 主要関数の動作検証
- 境界値テスト・異常系テスト

#### テスト対象関数の選定基準
```c
// 優先度：高
- 公開API関数
- メモリ操作を行う関数
- 状態遷移を管理する関数
- 計算ロジックを含む関数

// 優先度：中
- 初期化・終了処理関数
- データ変換関数
- 検証・バリデーション関数

// 優先度：低
- 単純なGetter/Setter
- ログ出力関数
- デバッグ用関数
```

#### テストコード例
```c
#include "unity.h"
#include "EventManager.h"

void setUp(void) {
    // テスト前の初期化
}

void tearDown(void) {
    // テスト後のクリーンアップ
}

void test_event_manager_register_handler(void) {
    // 正常系テスト
    EventManager* manager = event_manager_get_instance();
    bool result = event_manager_register_handler(manager, EVENT_TOUCH, dummy_handler);
    TEST_ASSERT_TRUE(result);
    
    // 異常系テスト
    result = event_manager_register_handler(NULL, EVENT_TOUCH, dummy_handler);
    TEST_ASSERT_FALSE(result);
}

void test_gesture_detector_swipe_detection(void) {
    // 境界値テスト
    SwipeDirection direction = calculate_swipe_direction(49, 0, 49); // 閾値未満
    TEST_ASSERT_EQUAL(SWIPE_NONE, direction);
    
    direction = calculate_swipe_direction(50, 0, 50); // 閾値ちょうど
    TEST_ASSERT_EQUAL(SWIPE_RIGHT, direction);
}
```

---

## ツール設定

### Cppcheckの設定ファイル
**ファイル**: `.cppcheck`
```ini
# Cppcheck設定
enable=warning,style,performance,portability,information
suppressions-list=.cppcheck-suppressions
addon=misra.json
platform=unix64
language=c++
std=c++17
```

### Clang-Tidyの設定ファイル
**ファイル**: `.clang-tidy`
```yaml
Checks: >
  *,
  -google-*,
  -fuchsia-*,
  -llvm-header-guard,
  -misc-non-private-member-variables-in-classes
HeaderFilterRegex: '.*'
AnalyzeTemporaryDtors: false
CheckOptions:
  - key: readability-identifier-naming.ClassCase
    value: PascalCase
  - key: readability-identifier-naming.FunctionCase
    value: lower_case
  - key: readability-identifier-naming.VariableCase
    value: lower_case
```

### Unityテスト設定
**ファイル**: `test/unity_config.h`
```c
#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#define UNITY_INCLUDE_DOUBLE
#define UNITY_DOUBLE_PRECISION 1e-12
#define UNITY_EXCLUDE_FLOAT
#define UNITY_OUTPUT_CHAR(a) Serial.write(a)
#define UNITY_OUTPUT_FLUSH() Serial.flush()

#endif
```

---

## 品質基準・合格条件

### フェーズ別合格基準

| フェーズ | 合格条件 |
|---------|---------|
| コード生成 | コンパイル成功、基本機能動作確認 |
| Cppcheck | 警告0件、MISRA準拠率90%以上 |
| Clang-Tidy | 重要度高警告0件、中警告50%以下 |
| 単体テスト | テストカバレッジ80%以上、全テスト成功 |

### 品質メトリクス

```bash
# コードカバレッジ計測
gcov -b -c src/*.c
lcov --capture --directory . --output-file coverage.info

# 複雑度計測
pmccabe src/*.c src/*.cpp

# コード行数・品質指標
cloc src/
```

---

## 自動化スクリプト

### 品質チェック一括実行
**ファイル**: `scripts/quality_check.sh`
```bash
#!/bin/bash

echo "=== 品質チェック開始 ==="

# 1. Cppcheck実行
echo "--- Cppcheck実行 ---"
cppcheck --addon=misra.json --enable=all --error-exitcode=1 src/
if [ $? -ne 0 ]; then
    echo "ERROR: Cppcheckでエラーが検出されました"
    exit 1
fi

# 2. Clang-Tidy実行
echo "--- Clang-Tidy実行 ---"
clang-tidy src/*.cpp src/*.c -- -I./src -I./lib
if [ $? -ne 0 ]; then
    echo "WARNING: Clang-Tidyで警告が検出されました"
fi

# 3. ビルドテスト
echo "--- ビルドテスト ---"
platformio run
if [ $? -ne 0 ]; then
    echo "ERROR: ビルドに失敗しました"
    exit 1
fi

# 4. 単体テスト実行
echo "--- 単体テスト実行 ---"
platformio test
if [ $? -ne 0 ]; then
    echo "ERROR: 単体テストに失敗しました"
    exit 1
fi

echo "=== 品質チェック完了 ==="
```

---

## 運用ルール

### コミット前チェック
1. 必ず`quality_check.sh`を実行
2. 全てのチェックを通過してからコミット
3. テストカバレッジレポートを更新

### コードレビュー観点
- AIが生成したコードの妥当性
- ツールで検出できない論理的問題
- 仕様との整合性
- パフォーマンス要件の充足

### 継続改善
- 検出された問題パターンをAIプロンプトに反映
- 頻発する問題に対するチェックルール追加
- テストケースの拡充

---

## PlatformIO開発時の注意事項

### アップロード前の必須作業
1. **シリアルモニターのキル**
   - `pio run --target upload`実行前に必ず以下を実行：
   ```bash
   pkill -f "pio.*monitor" || true
   ```
   - シリアルモニターが開いているとアップロードが失敗するため

### 推奨コマンド
```bash
# アップロード時の推奨コマンド（モニターキル + アップロード）
pkill -f "pio.*monitor" || true; sleep 1; pio run --target upload
```

---

## Gitコミット規則

### コミットメッセージ
1. **使用言語**: 日本語で記述すること
2. **形式**: 
   ```
   機能追加：タッチ機能の実装
   
   - XPT2046タッチコントローラーのサポートを追加
   - タッチ座標のキャリブレーション設定
   - デバッグ表示機能の実装
   ```

3. **禁止事項**: 
   - 英語でのコミットメッセージは使用しない
   - AIが生成したことを示す署名は日本語のコミットメッセージの後に英語で記載

### コミット例
```bash
git commit -m "タッチ機能をESP32-2432S028R用に実装

- XPT2046タッチコントローラーのサポート追加
- 正しいSPIピン設定（CLK:25, DIN:32, DO:39, CS:33）
- タッチキャリブレーション値の設定
- offset_rotation=2で座標マッピングを修正

🤖 Generated with [Claude Code](https://claude.ai/code)

Co-Authored-By: Claude <noreply@anthropic.com>"
```

この開発フローにより、AI生成コードの品質を体系的に保証し、低コストで高品質な開発を実現します。