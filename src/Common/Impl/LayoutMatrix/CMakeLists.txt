set(TARGET_NAME LayoutMatrix)

add_library(${TARGET_NAME} STATIC LayoutMatrix.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})