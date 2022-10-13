set(TARGET_NAME LayoutReader)

add_library(${TARGET_NAME} STATIC LayoutReader.cpp)
target_link_libraries(${TARGET_NAME} PUBLIC RLE)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})