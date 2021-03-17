cmake_minimum_required(VERSION 3.14)

project(QTX3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(QT_INCLUDE ${QT_INCLUDE_DIRS} ${Qt${QT_VERSION_MAJOR}_INCLUDE_DIRS})
include_directories(${QT_INCLUDE})

# Tixi - the XML handling library
find_package(TIXI3 REQUIRED)
include_directories(${TIXI_INCLUDE_DIRS})

add_subdirectory(src)

add_library(QTX3 SHARED
  QTX3_global.h
)

target_link_libraries(QTX3 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QTX3 PRIVATE ${TIXI_LIBRARIES})
target_link_libraries(QTX3 PUBLIC QXmodel)
target_link_libraries(QTX3 PUBLIC Utils)
target_compile_definitions(QTX3 PRIVATE QTX3_LIBRARY)



add_subdirectory(demo)
add_subdirectory(test)

enable_testing()

find_package(PythonInterp)
if( NOT PYTHONINTERP_FOUND )
    message(ERROR "Could not find Python Interpreter. Will not be able to generate library header files")
endif()

set(INSTALL_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/qtx3.h")

set_target_properties(QTX3 PROPERTIES PUBLIC_HEADER "${TIXI_INCLUDE_DIRS}/tixi.h;${INSTALL_HEADER}")

install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_qtx3_h.py -i \"${INSTALL_HEADERS}\" -o ${INSTALL_HEADER})")
install(TARGETS QTX3)

