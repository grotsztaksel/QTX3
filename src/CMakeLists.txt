add_subdirectory(utils)
add_subdirectory(model)
add_subdirectory(widgets)


set(QTX3_INCLUDE ${UTILS_INCLUDE_DIR} ${MODEL_INCLUDE_DIR} ${WIDGETS_INCLUDE_DIR} PARENT_SCOPE)
