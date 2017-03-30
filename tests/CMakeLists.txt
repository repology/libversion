ADD_EXECUTABLE(compare_test compare_test.c)
TARGET_LINK_LIBRARIES(compare_test version)
ADD_TEST(compare_test compare_test)
