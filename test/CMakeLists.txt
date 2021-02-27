#find_package(GTest REQUIRED)

set(GTEST_ROOT "C:/Libs/gtest")
set(GTEST_LIBRARY "${GTEST_ROOT}/lib")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")

add_subdirectory(utils)
add_subdirectory(model)
add_subdirectory(widgets)
