#include <unity.h>

// 基本的なテストケース
void test_basic_math(void) {
    TEST_ASSERT_EQUAL(4, 2 + 2);
}

void test_string_operations(void) {
    TEST_ASSERT_EQUAL_STRING("Hello", "Hello");
}

void setUp(void) {
    // テスト前の初期化処理
}

void tearDown(void) {
    // テスト後のクリーンアップ処理
}

void setup() {
    UNITY_BEGIN();
    
    RUN_TEST(test_basic_math);
    RUN_TEST(test_string_operations);
    
    UNITY_END();
}

void loop() {
    // テスト後は何もしない
}