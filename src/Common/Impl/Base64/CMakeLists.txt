set(TARGET_NAME Base64)

add_library(${TARGET_NAME} Base64.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})