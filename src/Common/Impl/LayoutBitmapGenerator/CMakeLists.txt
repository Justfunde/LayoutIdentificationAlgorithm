set(TARGET_NAME LayoutProcessor)

add_library(${TARGET_NAME} STATIC LayoutBitmapGenerator.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})