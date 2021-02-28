file(GLOB_RECURSE SRC_MODEL *.cpp *.h)

add_library(QXmodel OBJECT ${SRC_MODEL})

target_include_directories(QXmodel PRIVATE ${Utils_INCLUDE_DIRS})
target_link_libraries(QXmodel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QXmodel PRIVATE ${TIXI_LIBRARIES})
