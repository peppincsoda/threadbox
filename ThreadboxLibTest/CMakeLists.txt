cmake_minimum_required(VERSION 3.17.2)

project(ThreadboxLibTest)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    ThreadboxLibTest
    main.cpp
)
target_link_libraries(
    ThreadboxLibTest
    LINK_PUBLIC ThreadboxLib
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(ThreadboxLibTest)
