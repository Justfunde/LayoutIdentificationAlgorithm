set(TARGET_NAME BitMatrix)

add_library(${TARGET_NAME} BitMatrix.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})