set(TARGET_NAME RLE)

add_library(${TARGET_NAME} STATIC RunLengthEncoding.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})