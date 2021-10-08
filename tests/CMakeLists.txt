add_executable(locked-storage-test locked-storage-tests.cpp)

target_link_libraries(locked-storage-test ${CONAN_LIBS} frameworkd)

include(GoogleTest)
gtest_discover_tests(locked-storage-test)
