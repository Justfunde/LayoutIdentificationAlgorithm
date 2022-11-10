set(TARGET_NAME LayoutConverter)

add_library(${TARGET_NAME} STATIC LayoutConverter.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})