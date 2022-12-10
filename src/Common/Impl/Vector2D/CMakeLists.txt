set(TARGET_NAME Vector2D)

add_library(${TARGET_NAME} STATIC Vector2D.cpp Template.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})