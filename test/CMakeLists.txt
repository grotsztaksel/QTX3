#find_package(GTest REQUIRED)

set(GTEST_ROOT "C:/Lib/GTest")
set(GTEST_LIBRARY "${GTEST_ROOT}/lib")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")

find_package(Qt5Test REQUIRED)

add_subdirectory(utils)
add_subdirectory(model)
add_subdirectory(widgets)

include_directories(PRIVATE ${QTX3_INCLUDE} ${TEST_UTIL_INCLUDE} ${TEST_MODEL_INCLUDE} ${TEST_WIDGETS_INCLUDE})

add_executable(qtx3_Gtests  ${GTEST_MODEL_SRC} ${TEST_MODEL_COMMON_SRC} ${GTEST_UTILS_SRC} ${TEST_UTILS_COMMON_SRC} ${GTEST_WIDGETS_SRC} ${TEST_WIDGETS_COMMON_SRC})

target_link_libraries(qtx3_Gtests PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qtx3_Gtests PRIVATE QTX3)
target_link_libraries(qtx3_Gtests PRIVATE ${TIXI_LIBRARIES})

target_include_directories(qtx3_Gtests PRIVATE ${GTEST_INCLUDE_DIR})
target_link_directories(qtx3_Gtests PRIVATE ${GTEST_LIBRARY})
target_link_libraries(qtx3_Gtests PRIVATE gtest_main gtest)

add_test(NAME qtx3_Gtests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND qtx3_tests)


add_executable(qtx3_Qtests  ${QTEST_MODEL_SRC} ${TEST_MODEL_COMMON_SRC} ${QTEST_UTILS_SRC} ${TEST_UTILS_COMMON_SRC} ${QTEST_WIDGETS_SRC} ${TEST_WIDGETS_COMMON_SRC})

target_link_libraries(qtx3_Qtests PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qtx3_Qtests PRIVATE QTX3)
target_link_libraries(qtx3_Qtests PRIVATE ${TIXI_LIBRARIES})

target_link_libraries(qtx3_Qtests PRIVATE Qt5::Test)

add_test(NAME qtx3_Qtests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND qtx3_Qtests)
