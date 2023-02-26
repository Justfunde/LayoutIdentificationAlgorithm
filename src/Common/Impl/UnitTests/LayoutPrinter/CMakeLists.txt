set(TARGET_NAME LayoutPrinter)

add_library(${TARGET_NAME} STATIC LayoutPrinter.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})