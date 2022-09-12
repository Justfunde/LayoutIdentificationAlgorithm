set(TARGET_NAME ByteHandler)

add_library(${TARGET_NAME} STATIC ByteHandler.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})