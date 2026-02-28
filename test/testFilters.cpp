//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <unity.h>

void test_dummy()
{
    TEST_ASSERT_TRUE(true);
}

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_dummy);
    return UNITY_END();
}