file(GLOB_RECURSE MODEL_SRC *.cpp )
file(GLOB_RECURSE MODEL_HEADERS  *.h)

add_library(QXmodel OBJECT ${MODEL_SRC} ${MODEL_HEADERS} ${QTX3_HDR})

target_include_directories(QXmodel PUBLIC ${QTX3_INC})
target_include_directories(QXmodel PRIVATE ${UTILS_INCLUDE_DIR})
target_link_libraries(QXmodel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QXmodel PRIVATE ${TIXI_LIBRARIES})

set(MODEL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(MODEL_HEADERS ${MODEL_HEADERS} PARENT_SCOPE)
