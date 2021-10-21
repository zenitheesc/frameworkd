add_executable(daemon-test daemon-test.cpp ../src/classes/dbus-handler/dbus-handler.cpp)

target_link_libraries(daemon-test ${CONAN_LIBS})

include(GoogleTest)
gtest_discover_tests(daemon-test)
