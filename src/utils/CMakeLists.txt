add_library(Utils OBJECT
  tixi_utils.cpp
  tixi_utils.h
)

set(Utils_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
