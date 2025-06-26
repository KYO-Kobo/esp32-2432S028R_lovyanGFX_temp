# ESP32-2432S028R Development Environment Setup
# 一から開発環境をセットアップするためのMakefile

# カラー出力
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
NC := \033[0m # No Color

# プラットフォーム検出
UNAME_S := $(shell uname -s)
ARCH := $(shell uname -m)

.PHONY: help setup setup-brew setup-python setup-platformio setup-claude setup-github setup-quality-tools setup-project verify clean

# デフォルトターゲット
all: help

# ヘルプ表示
help:
	@echo "$(BLUE)ESP32-2432S028R 開発環境セットアップ$(NC)"
	@echo ""
	@echo "$(GREEN)利用可能なコマンド:$(NC)"
	@echo "  $(YELLOW)make setup$(NC)              - 開発環境の完全セットアップ"
	@echo "  $(YELLOW)make setup-brew$(NC)         - Homebrewのインストール (macOS)"
	@echo "  $(YELLOW)make setup-python$(NC)       - Python環境のセットアップ"
	@echo "  $(YELLOW)make setup-platformio$(NC)   - PlatformIOのインストール"
	@echo "  $(YELLOW)make setup-claude$(NC)       - Claude Codeのインストール"
	@echo "  $(YELLOW)make setup-github$(NC)       - GitHub CLIのセットアップ"
	@echo "  $(YELLOW)make setup-quality-tools$(NC) - 品質チェックツールのインストール"
	@echo "  $(YELLOW)make setup-project$(NC)      - プロジェクト依存関係のインストール"
	@echo "  $(YELLOW)make verify$(NC)             - インストール状況の確認"
	@echo "  $(YELLOW)make clean$(NC)              - 一時ファイルのクリーンアップ"
	@echo ""
	@echo "$(GREEN)品質チェックコマンド:$(NC)"
	@echo "  $(YELLOW)pio run -e quality-check$(NC) - 全品質チェック実行"
	@echo "  $(YELLOW)pio run -e cppcheck$(NC)      - Cppcheck静的解析"
	@echo "  $(YELLOW)pio run -e clang-tidy$(NC)    - Clang-Tidy解析"
	@echo "  $(YELLOW)pio test$(NC)                 - 単体テスト実行"

# 完全セットアップ
setup: 
	@echo "$(BLUE)=== 開発環境セットアップ開始 ===$(NC)"
	@$(MAKE) setup-brew
	@$(MAKE) setup-python
	@$(MAKE) setup-platformio
	@$(MAKE) setup-claude
	@$(MAKE) setup-github
	@$(MAKE) setup-quality-tools
	@$(MAKE) setup-project
	@$(MAKE) verify
	@echo "$(GREEN)🎉 開発環境セットアップ完了! 🎉$(NC)"
	@echo "$(GREEN)以下のコマンドでプロジェクトを開始できます:$(NC)"
	@echo "  pio run                    # ビルド"
	@echo "  pio run --target upload    # アップロード"
	@echo "  pio run -e quality-check   # 品質チェック"

# Homebrewのセットアップ (macOS)
setup-brew:
	@echo "$(BLUE)=== Homebrewセットアップ ===$(NC)"
ifeq ($(UNAME_S),Darwin)
	@if ! command -v brew >/dev/null 2>&1; then \
		echo "$(YELLOW)Homebrewをインストールしています...$(NC)"; \
		/bin/bash -c "$$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"; \
		echo "$(GREEN)✓ Homebrew インストール完了$(NC)"; \
	else \
		echo "$(GREEN)✓ Homebrew 既にインストール済み$(NC)"; \
	fi
	@echo "$(YELLOW)Homebrewを更新しています...$(NC)"
	@brew update || true
	@echo "$(GREEN)✓ Homebrew セットアップ完了$(NC)"
else
	@echo "$(YELLOW)macOS以外のため、Homebrewセットアップをスキップします$(NC)"
endif

# Python環境のセットアップ
setup-python:
	@echo "$(BLUE)=== Python環境セットアップ ===$(NC)"
	@if ! command -v python3 >/dev/null 2>&1; then \
		echo "$(RED)Python3が見つかりません$(NC)"; \
		if [ "$(UNAME_S)" = "Darwin" ]; then \
			echo "$(YELLOW)Python3をインストールしています...$(NC)"; \
			brew install python3; \
		else \
			echo "$(RED)手動でPython3をインストールしてください$(NC)"; \
			exit 1; \
		fi; \
	else \
		echo "$(GREEN)✓ Python3 インストール済み$(NC)"; \
	fi
	@if ! command -v pip3 >/dev/null 2>&1; then \
		echo "$(YELLOW)pip3をインストールしています...$(NC)"; \
		python3 -m ensurepip --upgrade; \
	else \
		echo "$(GREEN)✓ pip3 インストール済み$(NC)"; \
	fi
	@echo "$(GREEN)✓ Python環境セットアップ完了$(NC)"

# PlatformIOのセットアップ
setup-platformio:
	@echo "$(BLUE)=== PlatformIOセットアップ ===$(NC)"
	@if ! command -v pio >/dev/null 2>&1; then \
		echo "$(YELLOW)PlatformIOをインストールしています...$(NC)"; \
		pip3 install --upgrade platformio; \
		echo "$(GREEN)✓ PlatformIO インストール完了$(NC)"; \
	else \
		echo "$(GREEN)✓ PlatformIO 既にインストール済み$(NC)"; \
		echo "$(YELLOW)PlatformIOを更新しています...$(NC)"; \
		pip3 install --upgrade platformio; \
	fi
	@echo "$(GREEN)✓ PlatformIOセットアップ完了$(NC)"

# Claude Codeのセットアップ
setup-claude:
	@echo "$(BLUE)=== Claude Codeセットアップ ===$(NC)"
	@if ! command -v claude >/dev/null 2>&1; then \
		echo "$(YELLOW)Claude Codeをインストールしています...$(NC)"; \
		if [ "$(UNAME_S)" = "Darwin" ]; then \
			brew install claude; \
		else \
			echo "$(YELLOW)Claude CodeのLinux版インストール...$(NC)"; \
			curl -fsSL https://claude.ai/install.sh | bash; \
		fi; \
		echo "$(GREEN)✓ Claude Code インストール完了$(NC)"; \
	else \
		echo "$(GREEN)✓ Claude Code 既にインストール済み$(NC)"; \
	fi
	@echo "$(YELLOW)Claude Codeの設定を確認してください:$(NC)"
	@echo "  claude auth login"
	@echo "$(GREEN)✓ Claude Codeセットアップ完了$(NC)"

# GitHub CLIのセットアップ
setup-github:
	@echo "$(BLUE)=== GitHub CLIセットアップ ===$(NC)"
	@if ! command -v gh >/dev/null 2>&1; then \
		echo "$(YELLOW)GitHub CLIをインストールしています...$(NC)"; \
		if [ "$(UNAME_S)" = "Darwin" ]; then \
			brew install gh; \
		else \
			echo "$(YELLOW)GitHub CLIのLinux版インストール...$(NC)"; \
			curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | sudo dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg; \
			echo "deb [arch=$$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | sudo tee /etc/apt/sources.list.d/github-cli.list > /dev/null; \
			sudo apt update && sudo apt install gh; \
		fi; \
		echo "$(GREEN)✓ GitHub CLI インストール完了$(NC)"; \
	else \
		echo "$(GREEN)✓ GitHub CLI 既にインストール済み$(NC)"; \
	fi
	@echo "$(YELLOW)GitHub CLIの認証を設定してください:$(NC)"
	@echo "  gh auth login"
	@echo "$(GREEN)✓ GitHub CLIセットアップ完了$(NC)"

# 品質チェックツールのセットアップ
setup-quality-tools:
	@echo "$(BLUE)=== 品質チェックツールセットアップ ===$(NC)"
	@if [ "$(UNAME_S)" = "Darwin" ]; then \
		if ! command -v cppcheck >/dev/null 2>&1; then \
			echo "$(YELLOW)Cppcheckをインストールしています...$(NC)"; \
			brew install cppcheck; \
		else \
			echo "$(GREEN)✓ Cppcheck 既にインストール済み$(NC)"; \
		fi; \
		if ! command -v clang-tidy >/dev/null 2>&1; then \
			echo "$(YELLOW)LLVM (clang-tidy)をインストールしています...$(NC)"; \
			brew install llvm; \
			echo "$(YELLOW)LLVMのPATHを設定しています...$(NC)"; \
			if ! grep -q "llvm/bin" ~/.zshrc 2>/dev/null; then \
				echo 'export PATH="/opt/homebrew/opt/llvm/bin:$$PATH"' >> ~/.zshrc; \
			fi; \
		else \
			echo "$(GREEN)✓ LLVM (clang-tidy) 既にインストール済み$(NC)"; \
		fi; \
		if ! command -v lcov >/dev/null 2>&1; then \
			echo "$(YELLOW)lcov (カバレッジツール) をインストールしています...$(NC)"; \
			brew install lcov; \
		else \
			echo "$(GREEN)✓ lcov 既にインストール済み$(NC)"; \
		fi; \
	else \
		echo "$(YELLOW)Linux環境での品質ツールインストール...$(NC)"; \
		sudo apt update; \
		sudo apt install -y cppcheck clang-tidy lcov; \
	fi
	@echo "$(GREEN)✓ 品質チェックツールセットアップ完了$(NC)"

# プロジェクト依存関係のセットアップ
setup-project:
	@echo "$(BLUE)=== プロジェクト依存関係セットアップ ===$(NC)"
	@if [ -f "platformio.ini" ]; then \
		echo "$(YELLOW)PlatformIO依存関係をインストールしています...$(NC)"; \
		pio pkg install; \
		echo "$(GREEN)✓ PlatformIO依存関係インストール完了$(NC)"; \
	else \
		echo "$(YELLOW)platformio.iniが見つかりません。スキップします$(NC)"; \
	fi
	@if [ -f "package.json" ]; then \
		echo "$(YELLOW)Node.js依存関係をインストールしています...$(NC)"; \
		npm install; \
		echo "$(GREEN)✓ Node.js依存関係インストール完了$(NC)"; \
	fi
	@if [ -f "requirements.txt" ]; then \
		echo "$(YELLOW)Python依存関係をインストールしています...$(NC)"; \
		pip3 install -r requirements.txt; \
		echo "$(GREEN)✓ Python依存関係インストール完了$(NC)"; \
	fi
	@echo "$(GREEN)✓ プロジェクト依存関係セットアップ完了$(NC)"

# インストール確認
verify:
	@echo "$(BLUE)=== インストール状況確認 ===$(NC)"
	@echo "$(YELLOW)開発ツール:$(NC)"
	@if command -v python3 >/dev/null 2>&1; then \
		echo "$(GREEN)✓ Python3: $$(python3 --version)$(NC)"; \
	else \
		echo "$(RED)✗ Python3: インストールされていません$(NC)"; \
	fi
	@if command -v pio >/dev/null 2>&1; then \
		echo "$(GREEN)✓ PlatformIO: $$(pio --version | head -1)$(NC)"; \
	else \
		echo "$(RED)✗ PlatformIO: インストールされていません$(NC)"; \
	fi
	@if command -v claude >/dev/null 2>&1; then \
		echo "$(GREEN)✓ Claude Code: インストール済み$(NC)"; \
	else \
		echo "$(RED)✗ Claude Code: インストールされていません$(NC)"; \
	fi
	@if command -v gh >/dev/null 2>&1; then \
		echo "$(GREEN)✓ GitHub CLI: $$(gh --version | head -1)$(NC)"; \
	else \
		echo "$(RED)✗ GitHub CLI: インストールされていません$(NC)"; \
	fi
	@echo ""
	@echo "$(YELLOW)品質チェックツール:$(NC)"
	@if command -v cppcheck >/dev/null 2>&1; then \
		echo "$(GREEN)✓ Cppcheck: $$(cppcheck --version)$(NC)"; \
	else \
		echo "$(RED)✗ Cppcheck: インストールされていません$(NC)"; \
	fi
	@if command -v clang-tidy >/dev/null 2>&1; then \
		echo "$(GREEN)✓ Clang-Tidy: $$(clang-tidy --version | head -1)$(NC)"; \
	elif [ -f "/opt/homebrew/opt/llvm/bin/clang-tidy" ]; then \
		echo "$(GREEN)✓ Clang-Tidy: インストール済み (PATH要設定)$(NC)"; \
	else \
		echo "$(RED)✗ Clang-Tidy: インストールされていません$(NC)"; \
	fi
	@if command -v lcov >/dev/null 2>&1; then \
		echo "$(GREEN)✓ lcov: $$(lcov --version | head -1)$(NC)"; \
	else \
		echo "$(YELLOW)⚠ lcov: インストールされていません (オプション)$(NC)"; \
	fi
	@echo ""
	@echo "$(YELLOW)認証状況:$(NC)"
	@if claude auth whoami >/dev/null 2>&1; then \
		echo "$(GREEN)✓ Claude Code: 認証済み$(NC)"; \
	else \
		echo "$(YELLOW)⚠ Claude Code: 認証が必要 (claude auth login)$(NC)"; \
	fi
	@if gh auth status >/dev/null 2>&1; then \
		echo "$(GREEN)✓ GitHub CLI: 認証済み$(NC)"; \
	else \
		echo "$(YELLOW)⚠ GitHub CLI: 認証が必要 (gh auth login)$(NC)"; \
	fi

# クリーンアップ
clean:
	@echo "$(BLUE)=== クリーンアップ ===$(NC)"
	@if [ -d ".pio" ]; then \
		echo "$(YELLOW)PlatformIOビルドファイルを削除しています...$(NC)"; \
		rm -rf .pio; \
	fi
	@if [ -d "node_modules" ]; then \
		echo "$(YELLOW)Node.jsモジュールを削除しています...$(NC)"; \
		rm -rf node_modules; \
	fi
	@if [ -d "__pycache__" ]; then \
		echo "$(YELLOW)Pythonキャッシュを削除しています...$(NC)"; \
		find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true; \
	fi
	@if [ -d "logs" ]; then \
		echo "$(YELLOW)ログファイルを削除しています...$(NC)"; \
		rm -rf logs; \
	fi
	@if [ -d "coverage" ]; then \
		echo "$(YELLOW)カバレッジファイルを削除しています...$(NC)"; \
		rm -rf coverage; \
	fi
	@echo "$(GREEN)✓ クリーンアップ完了$(NC)"

# 新規プロジェクト作成
new-project:
	@echo "$(BLUE)=== 新規プロジェクト作成 ===$(NC)"
	@read -p "プロジェクト名を入力してください: " project_name; \
	if [ -n "$$project_name" ]; then \
		pio project init --board esp32dev --ide vscode --project-option "framework=arduino" --project-dir "$$project_name"; \
		echo "$(GREEN)✓ 新規プロジェクト '$$project_name' を作成しました$(NC)"; \
		echo "$(YELLOW)次のコマンドでプロジェクトディレクトリに移動してください:$(NC)"; \
		echo "  cd $$project_name"; \
	else \
		echo "$(RED)プロジェクト名が入力されませんでした$(NC)"; \
	fi

# 開発サーバー起動 (VSCode)
dev:
	@echo "$(BLUE)=== 開発環境起動 ===$(NC)"
	@if command -v code >/dev/null 2>&1; then \
		echo "$(YELLOW)VSCodeで開発環境を起動しています...$(NC)"; \
		code .; \
		echo "$(GREEN)✓ VSCode起動完了$(NC)"; \
	else \
		echo "$(RED)VSCodeがインストールされていません$(NC)"; \
		echo "$(YELLOW)手動でIDEを開いてください$(NC)"; \
	fi