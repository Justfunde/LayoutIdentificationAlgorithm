set(TARGET_NAME LayoutReader)

file(GLOB_RECURSE READER_SOURCE_LIST *.cpp)


add_library(${TARGET_NAME} STATIC ${READER_SOURCE_LIST})
target_link_libraries(${TARGET_NAME} PUBLIC RLE)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})