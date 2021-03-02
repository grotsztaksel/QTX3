file(GLOB_RECURSE SRC_MODEL *.cpp *.h)

add_library(QXmodel OBJECT ${SRC_MODEL})

target_include_directories(QXmodel PRIVATE ${UTILS_INCLUDE_DIR})
target_link_libraries(QXmodel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QXmodel PRIVATE ${TIXI_LIBRARIES})

set(MODEL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
