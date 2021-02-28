

add_library(Utils OBJECT
  txutils.cpp
  txutils.h
)

set(Utils_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
