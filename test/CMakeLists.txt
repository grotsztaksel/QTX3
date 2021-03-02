#find_package(GTest REQUIRED)

set(GTEST_ROOT "C:/Libs/gtest")
set(GTEST_LIBRARY "${GTEST_ROOT}/lib")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")



add_subdirectory(utils)
add_subdirectory(model)
add_subdirectory(widgets)

add_executable(qtx3_tests  ${TEST_MODEL_SRC} ${TEST_UTILS_SRC})
target_include_directories(qtx3_tests PRIVATE ${QTX3_INCLUDE})
target_link_libraries(qtx3_tests PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qtx3_tests PRIVATE QXTree)
target_link_libraries(qtx3_tests PRIVATE ${TIXI_LIBRARIES})

target_include_directories(qtx3_tests PRIVATE ${GTEST_INCLUDE_DIR})
target_link_directories(qtx3_tests PRIVATE ${GTEST_LIBRARY})
target_link_libraries(qtx3_tests PRIVATE gtest_main gtest)

add_test(NAME qtx3_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND qtx3_tests)
