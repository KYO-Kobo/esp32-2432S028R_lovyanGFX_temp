#!/usr/bin/env python3
"""
Coverage Generator for PlatformIO
"""

Import("env")
import subprocess
import sys
from pathlib import Path

def generate_coverage():
    """カバレッジレポート生成"""
    print("\n" + "="*50)
    print("📊 Code Coverage Analysis")
    print("="*50)
    
    coverage_dir = Path("coverage")
    coverage_dir.mkdir(exist_ok=True)
    
    build_dir = Path(".pio/build")
    if not build_dir.exists():
        print("❌ Build directory not found. Run tests first.")
        return
    
    # lcovを使用したカバレッジ計測
    lcov_available = subprocess.run(["which", "lcov"], capture_output=True).returncode == 0
    
    if lcov_available:
        print("Using lcov for coverage analysis...")
        
        # カバレッジデータ収集
        print("Collecting coverage data...")
        result1 = subprocess.run([
            "lcov",
            "--capture",
            "--directory", str(build_dir),
            "--output-file", "coverage/coverage.info"
        ], capture_output=True, text=True)
        
        if result1.returncode == 0:
            print("✅ Coverage data collected")
            
            # 不要なファイルを除外
            print("Filtering coverage data...")
            subprocess.run([
                "lcov",
                "--remove", "coverage/coverage.info",
                "/usr/*", "*/test/*", "*/lib/*",
                "--output-file", "coverage/coverage_filtered.info"
            ], capture_output=True)
            
            # HTMLレポート生成
            print("Generating HTML report...")
            result2 = subprocess.run([
                "genhtml",
                "coverage/coverage_filtered.info",
                "--output-directory", "coverage/html"
            ], capture_output=True, text=True)
            
            if result2.returncode == 0:
                print("✅ HTML coverage report generated")
                print("📁 Report location: coverage/html/index.html")
            else:
                print("⚠️  HTML report generation failed")
                print(result2.stderr)
        else:
            print("⚠️  Coverage data collection failed")
            print(result1.stderr)
    
    else:
        print("lcov not available, using gcov...")
        
        # gcda ファイルを検索
        gcda_files = list(build_dir.rglob("*.gcda"))
        
        if gcda_files:
            print(f"Found {len(gcda_files)} gcda files")
            
            for gcda_file in gcda_files:
                print(f"Processing: {gcda_file.name}")
                subprocess.run([
                    "gcov", str(gcda_file)
                ], capture_output=True)
            
            print("✅ gcov coverage analysis completed")
            print("📁 .gcov files generated in current directory")
        else:
            print("❌ No coverage data found. Ensure tests are built with coverage flags.")
    
    print("Coverage analysis completed")
    print("="*50)

# PlatformIOから呼び出された場合
if __name__ == "__main__":
    generate_coverage()
else:
    # extra_scriptsから呼び出された場合
    generate_coverage()