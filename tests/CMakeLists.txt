add_executable(compare_test compare_test.c)
target_link_libraries(compare_test libversion)
add_test(compare_test compare_test)

add_executable(signature_test signature_test.c)
target_link_libraries(signature_test libversion)
add_test(signature_test signature_test)

add_executable(compare_fuzzer compare_fuzzer.c)
target_link_libraries(compare_fuzzer libversion)
