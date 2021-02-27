file(GLOB_RECURSE CPP_SRCUNITTESTS *.cpp *.h)

set(TESTED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
string(REGEX REPLACE "/test/" "/src/" TESTED_SRC_DIR ${TESTED_SRC_DIR})

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}  is a test dir for ${TESTED_SRC_DIR}")
file(GLOB_RECURSE CPP_SRC "${TESTED_SRC_DIR}/*cpp")


# Build the test executable
add_executable(qxtree_test EXCLUDE_FROM_ALL ${CPP_SRCUNITTESTS})

# Include & link gtest
target_include_directories(qxtree_test PRIVATE ${GTEST_INCLUDE_DIR})
target_link_directories(qxtree_test PRIVATE ${GTEST_LIBRARY})
target_link_libraries(qxtree_test PRIVATE gtest)

# Include the headers from the module
target_include_directories(qxtree_test PRIVATE "${TESTED_SRC_DIR}")

# Link the whole library
target_link_libraries(qxtree_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qxtree_test PRIVATE QXTree)

add_test(NAME modeltests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND qxtree_test)
