set(TARGET_NAME CRC32)

add_library(${TARGET_NAME} STATIC Crc32.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})