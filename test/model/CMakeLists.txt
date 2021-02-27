file(GLOB_RECURSE CPP_SRCUNITTESTS *.cpp *.h)


# Build the test executable
add_executable(qxtree_test EXCLUDE_FROM_ALL ${CPP_SRCUNITTESTS})

target_include_directories(qxtree_test PRIVATE ${GTEST_INCLUDE_DIR})
target_link_directories(qxtree_test PRIVATE ${GTEST_LIBRARY})

target_include_directories(qxtree_test PRIVATE "../../src/model")
target_link_directories(qxtree_test PRIVATE QXmodel)

target_link_libraries(qxtree_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qxtree_test PRIVATE ${TIXI_LIBRARIES})


add_test(NAME modeltests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND qxtree_test)
