
file(GLOB_RECURSE MODEL_SRC *.cpp *.h *.ui)

include_directories(${QT_INCLUDE})
include_directories(${MODEL_INCLUDE_DIR})

add_executable(qtx3demo  ${MODEL_SRC})

target_link_libraries(qtx3demo PRIVATE QTX3 PRIVATE)
target_link_libraries(qtx3demo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qtx3demo PRIVATE ${TIXI_LIBRARIES})
