cmake_minimum_required(VERSION 3.14)

project(QXTree LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_library(QXTree SHARED
  QXTree_global.h
  qxtreenode.cpp
  qxtreenode.h
  xpathtreemodel.cpp
  xpathtreemodel.h
  tixi_utils.cpp
  tixi_utils.h
)

# Tixi - the XML handling library
find_package(TIXI3 REQUIRED)
include_directories(${TIXI_INCLUDE_DIRS})

target_link_libraries(QXTree PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QXTree PRIVATE ${TIXI_LIBRARIES})

target_compile_definitions(QXTree PRIVATE QXTREE_LIBRARY)
