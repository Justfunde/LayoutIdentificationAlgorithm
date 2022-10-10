set(SOURCES_LIST BitMatrixTest.cpp)

set(LIBS_LIST LayoutMatrix BitMatrix Base64 RLE Crc32 ByteHandler)

add_unit_test(
   NAME Test__BitMatrixFuzzing
   SOURCES ${SOURCES_LIST}
   LIBRARIES ${LIBS_LIST})

add_unit_test(
   NAME Test__BitMatrixEncodeFuzzing
   SOURCES ${SOURCES_LIST}
   LIBRARIES ${LIBS_LIST})
