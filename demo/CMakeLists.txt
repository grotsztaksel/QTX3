
file(GLOB_RECURSE SRC_MODEL *.cpp *.h *.ui)

include_directories(${QT_INCLUDE})
include_directories(${MODEL_INCLUDE_DIR})

add_executable(qtx3demo  ${SRC_MODEL})

target_link_libraries(qtx3demo PRIVATE QXTree PRIVATE)
target_link_libraries(qtx3demo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qtx3demo PRIVATE ${TIXI_LIBRARIES})
