#!/usr/bin/env python3
"""
Clang-Tidy Runner for PlatformIO
"""

Import("env")
import subprocess
import sys
import os
from pathlib import Path

# LLVM PATH設定（Homebrewのkeg-only対応）
if '/opt/homebrew/opt/llvm/bin' not in os.environ.get('PATH', ''):
    os.environ['PATH'] = '/opt/homebrew/opt/llvm/bin:' + os.environ.get('PATH', '')

def run_clang_tidy():
    """Clang-Tidy実行"""
    print("\n" + "="*50)
    print("🔧 Clang-Tidy Static Analysis")
    print("="*50)
    
    src_dir = Path("src")
    if not src_dir.exists():
        print("❌ src/ directory not found")
        return
    
    # ソースファイル検索
    source_files = []
    source_files.extend(src_dir.glob("*.cpp"))
    source_files.extend(src_dir.glob("*.c"))
    
    if not source_files:
        print("❌ No source files found")
        return
    
    print(f"Found {len(source_files)} source files")
    
    # 各ファイルをチェック
    warning_count = 0
    
    for source_file in source_files:
        print(f"Checking: {source_file.name}...")
        
        cmd = [
            "clang-tidy",
            str(source_file),
            "--",
            "-Isrc",
            "-I.pio/libdeps/esp32-2432S028R/TFT_eSPI",
            "-std=c++17"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0 or result.stdout.strip():
            warning_count += 1
            print(f"⚠️  Warnings in {source_file.name}")
            if result.stdout:
                print(result.stdout)
        else:
            print(f"✅ {source_file.name}: OK")
    
    print(f"\nClang-Tidy analysis completed")
    if warning_count == 0:
        print("✅ No warnings found")
    else:
        print(f"⚠️  {warning_count} files with warnings")
    
    print("="*50)

# PlatformIOから呼び出された場合
if __name__ == "__main__":
    run_clang_tidy()
else:
    # extra_scriptsから呼び出された場合
    run_clang_tidy()