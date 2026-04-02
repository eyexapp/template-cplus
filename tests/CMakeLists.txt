find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(greeting_service_test greeting_service_test.cc)
target_link_libraries(greeting_service_test PRIVATE app_lib GTest::gtest_main)
gtest_discover_tests(greeting_service_test)

add_executable(router_test router_test.cc)
target_link_libraries(router_test PRIVATE app_lib GTest::gtest_main)
gtest_discover_tests(router_test)
