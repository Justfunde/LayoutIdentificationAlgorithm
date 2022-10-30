set(TARGET_NAME MskReader)

add_library(${TARGET_NAME} STATIC LayoutReader_MSK.cpp)
target_link_libraries(${TARGET_NAME} PUBLIC  LayoutReader RLE)
target_include_directories(${TARGET_NAME} PUBLIC ${SHARED_INCLUDE_DIRS})