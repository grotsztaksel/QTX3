add_library(QXmodel OBJECT
    qxtreenode.cpp
    qxtreenode.h
    xpathtreemodel.cpp
    xpathtreemodel.h
)

target_include_directories(QXmodel PRIVATE ${Utils_INCLUDE_DIRS})
target_link_libraries(QXmodel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QXmodel PRIVATE ${TIXI_LIBRARIES})
