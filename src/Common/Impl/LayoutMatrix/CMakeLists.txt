set(TARGET_NAME LayoutMatrix)

add_library(${TARGET_NAME} STATIC LayoutMatrix.cpp)
target_link_libraries(${TARGET_NAME} PUBLIC RLE_MATRIX_INCLUDED)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})