set(TARGET_NAME LayoutFragment)

add_library(${TARGET_NAME} STATIC LayoutFragment.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})