#!/usr/bin/env python3
"""
Cppcheck Runner for PlatformIO
"""

Import("env")
import subprocess
import sys
from pathlib import Path

def run_cppcheck():
    """Cppcheck実行"""
    print("\n" + "="*50)
    print("🔍 Cppcheck Static Analysis")
    print("="*50)
    
    src_dir = Path("src")
    if not src_dir.exists():
        print("❌ src/ directory not found")
        return
    
    # 基本チェック
    cmd_basic = [
        "cppcheck",
        "--enable=warning,style,performance,portability",
        "--error-exitcode=1",
        str(src_dir)
    ]
    
    print("Running basic checks...")
    result = subprocess.run(cmd_basic, capture_output=True, text=True)
    
    if result.returncode == 0:
        print("✅ Basic checks: PASSED")
    else:
        print("⚠️  Basic checks: WARNINGS FOUND")
        print(result.stderr)
    
    # MISRA-Cチェック（misra.jsonが存在する場合）
    misra_file = Path("misra.json")
    if misra_file.exists():
        cmd_misra = [
            "cppcheck",
            "--addon=misra.json",
            "--enable=all",
            str(src_dir)
        ]
        
        print("Running MISRA-C checks...")
        result_misra = subprocess.run(cmd_misra, capture_output=True, text=True)
        
        if result_misra.returncode == 0:
            print("✅ MISRA-C checks: PASSED")
        else:
            print("⚠️  MISRA-C checks: VIOLATIONS FOUND")
            print(result_misra.stderr)
    else:
        print("ℹ️  misra.json not found, skipping MISRA-C checks")
    
    print("Cppcheck analysis completed")
    print("="*50)

# PlatformIOから呼び出された場合
if __name__ == "__main__":
    run_cppcheck()
else:
    # extra_scriptsから呼び出された場合
    run_cppcheck()