set(TARGET_NAME LayoutBitmapZond)

add_library(${TARGET_NAME} STATIC LayoutBitmapZond.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})