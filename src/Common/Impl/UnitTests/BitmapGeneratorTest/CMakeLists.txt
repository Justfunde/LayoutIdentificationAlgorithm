set(SOURCES_LIST BitmapGeneratorTest.cpp)

set(LIBS_LIST Vector2D LayoutMatrix BitMatrix Base64 RLE Crc32 ByteHandler ByteHandler Base64 RLE Crc32 BitMatrix LayoutMatrix LayoutProcessor LayoutFragment  LayoutReader)


add_unit_test(
   NAME Test__CarryCellNonModified
   SOURCES ${SOURCES_LIST}
   LIBRARIES ${LIBS_LIST}
   DEFINITIONS "TEST_FILES_DIR=\"${CMAKE_CURRENT_LIST_DIR}/TestFiles/\"")


#todo: does not link by add_unit_test. needs fixes.
target_link_libraries(Test__CarryCellNonModified PUBLIC ${LIBS_LIST})