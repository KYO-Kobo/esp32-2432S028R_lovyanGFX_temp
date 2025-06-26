#!/bin/bash

# ESP32-2432S028R 品質チェック自動化スクリプト
# AI + 無料ツール品質保証フロー

set -e  # エラー時に停止

# カラー出力設定
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# ログ設定
LOG_DIR="logs"
LOG_FILE="$LOG_DIR/quality_check_$(date +%Y%m%d_%H%M%S).log"

# 作業ディレクトリ設定
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# ログディレクトリ作成
mkdir -p "$LOG_DIR"

# ログ関数
log() {
    echo -e "$1" | tee -a "$LOG_FILE"
}

log_header() {
    log "\n${BLUE}=== $1 ===${NC}"
}

log_success() {
    log "${GREEN}✓ $1${NC}"
}

log_warning() {
    log "${YELLOW}⚠ $1${NC}"
}

log_error() {
    log "${RED}✗ $1${NC}"
}

# エラーハンドリング
cleanup() {
    if [ $? -ne 0 ]; then
        log_error "品質チェックが失敗しました"
        log "詳細ログ: $LOG_FILE"
        exit 1
    fi
}
trap cleanup EXIT

# ツール存在確認
check_tools() {
    log_header "ツール存在確認"
    
    local tools=("cppcheck" "clang-tidy" "platformio")
    local missing_tools=()
    
    for tool in "${tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            log_success "$tool インストール済み"
        else
            log_warning "$tool が見つかりません"
            missing_tools+=("$tool")
        fi
    done
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        log_error "以下のツールをインストールしてください:"
        for tool in "${missing_tools[@]}"; do
            case "$tool" in
                "cppcheck")
                    log "  brew install cppcheck"
                    ;;
                "clang-tidy")
                    log "  brew install llvm"
                    ;;
                "platformio")
                    log "  pip install platformio"
                    ;;
            esac
        done
        exit 1
    fi
    
    log_success "全ツール確認完了"
}

# Phase 1: ビルドテスト
build_test() {
    log_header "Phase 1: ビルドテスト"
    
    log "PlatformIOビルドを実行しています..."
    if platformio run >> "$LOG_FILE" 2>&1; then
        log_success "ビルド成功"
    else
        log_error "ビルドに失敗しました"
        log "ビルドログを確認してください: $LOG_FILE"
        exit 1
    fi
}

# Phase 2: Cppcheck静的解析
cppcheck_analysis() {
    log_header "Phase 2: Cppcheck静的解析"
    
    local cppcheck_log="$LOG_DIR/cppcheck.log"
    local error_count=0
    
    log "基本的な静的解析を実行しています..."
    if cppcheck --enable=warning,style,performance,portability --error-exitcode=1 src/ > "$cppcheck_log" 2>&1; then
        log_success "Cppcheck基本チェック完了"
    else
        error_count=$((error_count + 1))
        log_warning "Cppcheckで警告が検出されました"
        log "詳細: $cppcheck_log"
    fi
    
    # MISRA-Cチェック（misra.jsonが存在する場合）
    if [ -f "misra.json" ]; then
        log "MISRA-C準拠チェックを実行しています..."
        local misra_log="$LOG_DIR/misra.log"
        if cppcheck --addon=misra.json --enable=all src/ > "$misra_log" 2>&1; then
            log_success "MISRA-Cチェック完了"
        else
            error_count=$((error_count + 1))
            log_warning "MISRA-C違反が検出されました"
            log "詳細: $misra_log"
        fi
    else
        log_warning "misra.json が見つかりません。MISRA-Cチェックをスキップします"
    fi
    
    if [ $error_count -eq 0 ]; then
        log_success "Cppcheck解析完了（エラーなし）"
    else
        log_warning "Cppcheck解析完了（警告あり）"
    fi
}

# Phase 3: Clang-Tidy静的解析
clang_tidy_analysis() {
    log_header "Phase 3: Clang-Tidy静的解析"
    
    local clang_tidy_log="$LOG_DIR/clang-tidy.log"
    local file_count=0
    local error_count=0
    
    log "詳細な静的解析を実行しています..."
    
    # C++ファイルとCファイルを検索
    while IFS= read -r -d '' file; do
        file_count=$((file_count + 1))
        log "チェック中: $file"
        
        if ! clang-tidy "$file" -- -Isrc -I.pio/libdeps/esp32-2432S028R/TFT_eSPI >> "$clang_tidy_log" 2>&1; then
            error_count=$((error_count + 1))
            log_warning "警告: $file"
        fi
    done < <(find src -name "*.cpp" -o -name "*.c" -print0)
    
    log "チェック対象ファイル数: $file_count"
    if [ $error_count -eq 0 ]; then
        log_success "Clang-Tidy解析完了（警告なし）"
    else
        log_warning "Clang-Tidy解析完了（$error_count ファイルで警告）"
        log "詳細: $clang_tidy_log"
    fi
}

# Phase 4: 単体テスト
unit_tests() {
    log_header "Phase 4: 単体テスト"
    
    local test_log="$LOG_DIR/unittest.log"
    
    log "単体テストを実行しています..."
    if platformio test > "$test_log" 2>&1; then
        log_success "全単体テスト成功"
        
        # テスト結果のサマリーを表示
        if grep -q "test summary" "$test_log"; then
            log "\nテストサマリー:"
            grep -A 10 "test summary" "$test_log" | while read line; do
                log "  $line"
            done
        fi
    else
        log_error "単体テストに失敗しました"
        log "詳細: $test_log"
        exit 1
    fi
}

# Phase 5: コードカバレッジ計測
coverage_analysis() {
    log_header "Phase 5: コードカバレッジ計測"
    
    local coverage_dir="coverage"
    mkdir -p "$coverage_dir"
    
    if command -v lcov >/dev/null 2>&1; then
        log "lcovを使用してカバレッジを計測しています..."
        
        # カバレッジデータ収集
        if lcov --capture --directory .pio/build --output-file "$coverage_dir/coverage.info" >> "$LOG_FILE" 2>&1; then
            # 不要なファイルを除外
            lcov --remove "$coverage_dir/coverage.info" '/usr/*' '*/test/*' '*/lib/*' \
                --output-file "$coverage_dir/coverage_filtered.info" >> "$LOG_FILE" 2>&1
            
            # HTMLレポート生成
            if genhtml "$coverage_dir/coverage_filtered.info" --output-directory "$coverage_dir/html" >> "$LOG_FILE" 2>&1; then
                log_success "カバレッジレポート生成完了"
                log "レポート: $coverage_dir/html/index.html"
            else
                log_warning "HTMLレポート生成に失敗"
            fi
        else
            log_warning "カバレッジデータ収集に失敗"
        fi
    elif command -v gcov >/dev/null 2>&1; then
        log "gcovを使用してカバレッジを計測しています..."
        find .pio/build -name "*.gcda" -exec gcov {} \; >> "$LOG_FILE" 2>&1
        log_success "gcovカバレッジ計測完了"
    else
        log_warning "カバレッジツールが見つかりません (lcov または gcov)"
        log "インストール: brew install lcov"
    fi
}

# メトリクス収集
collect_metrics() {
    log_header "コード品質メトリクス"
    
    # コード行数統計
    local total_lines=$(find src -name "*.cpp" -o -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $1}')
    log "総コード行数: $total_lines"
    
    # ファイル統計
    local cpp_files=$(find src -name "*.cpp" | wc -l)
    local c_files=$(find src -name "*.c" | wc -l)
    local h_files=$(find src -name "*.h" | wc -l)
    
    log "C++ファイル: $cpp_files"
    log "Cファイル: $c_files"
    log "ヘッダファイル: $h_files"
    
    # 複雑度統計（pmccabeが利用可能な場合）
    if command -v pmccabe >/dev/null 2>&1; then
        log "\n複雑度TOP10:"
        pmccabe src/*.cpp src/*.c 2>/dev/null | sort -nr | head -10 | while read line; do
            log "  $line"
        done
    fi
}

# レポート生成
generate_report() {
    log_header "品質レポート生成"
    
    local report_file="$LOG_DIR/quality_report_$(date +%Y%m%d_%H%M%S).md"
    
    cat > "$report_file" << EOF
# 品質チェックレポート

**実行日時**: $(date)
**プロジェクト**: ESP32-2432S028R Event-Driven Architecture

## チェック結果サマリー

| フェーズ | 結果 | 詳細 |
|---------|------|------|
| ビルドテスト | ✓ 成功 | PlatformIOビルド正常完了 |
| Cppcheck | $([ -f "$LOG_DIR/cppcheck.log" ] && echo "⚠ 要確認" || echo "✓ 成功") | $([ -f "$LOG_DIR/cppcheck.log" ] && echo "詳細: logs/cppcheck.log" || echo "エラーなし") |
| Clang-Tidy | $([ -f "$LOG_DIR/clang-tidy.log" ] && echo "⚠ 要確認" || echo "✓ 成功") | $([ -f "$LOG_DIR/clang-tidy.log" ] && echo "詳細: logs/clang-tidy.log" || echo "警告なし") |
| 単体テスト | ✓ 成功 | 全テスト正常終了 |
| カバレッジ | $([ -d "coverage" ] && echo "✓ 計測完了" || echo "- スキップ") | $([ -d "coverage/html" ] && echo "レポート: coverage/html/index.html" || echo "gcovレポート生成") |

## 推奨アクション

$([ -f "$LOG_DIR/cppcheck.log" ] && echo "- [ ] Cppcheck警告の確認・修正")
$([ -f "$LOG_DIR/clang-tidy.log" ] && echo "- [ ] Clang-Tidy警告の確認・修正")
- [ ] カバレッジレポートの確認
- [ ] 複雑度の高い関数のリファクタリング検討

## 詳細ログ

- 総合ログ: $LOG_FILE
- 各種ツールログ: $LOG_DIR/
EOF

    log_success "品質レポート生成完了: $report_file"
}

# メイン処理
main() {
    log_header "ESP32-2432S028R 品質チェック開始"
    log "実行時刻: $(date)"
    log "作業ディレクトリ: $PROJECT_ROOT"
    log "ログファイル: $LOG_FILE"
    
    check_tools
    build_test
    cppcheck_analysis
    clang_tidy_analysis
    unit_tests
    coverage_analysis
    collect_metrics
    generate_report
    
    log_header "品質チェック完了"
    log_success "🎉 全フェーズが正常に完了しました！"
    log_success "コミット準備完了です"
    log "詳細レポート: $LOG_DIR/"
}

# 引数処理
case "${1:-all}" in
    "tools")
        check_tools
        ;;
    "build")
        build_test
        ;;
    "cppcheck")
        cppcheck_analysis
        ;;
    "clang-tidy")
        clang_tidy_analysis
        ;;
    "test")
        unit_tests
        ;;
    "coverage")
        coverage_analysis
        ;;
    "metrics")
        collect_metrics
        ;;
    "all"|"")
        main
        ;;
    *)
        echo "使用法: $0 [tools|build|cppcheck|clang-tidy|test|coverage|metrics|all]"
        exit 1
        ;;
esac