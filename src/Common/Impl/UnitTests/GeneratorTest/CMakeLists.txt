set(SOURCES_LIST GeneratorTest.cpp)

set(LIBS_LIST LayoutMatrix BitMatrix Vector2D Base64 RLE Crc32 ByteHandler LayoutConverter LayoutFragment  LayoutBitmapZond LayoutReader LayoutProcessor )

add_unit_test(
   NAME Test__OutMsk
   SOURCES ${SOURCES_LIST}
   LIBRARIES ${LIBS_LIST}
   DEFINITIONS "TEST_FILES_DIR=\"${CMAKE_CURRENT_LIST_DIR}/TestFiles/\"" "ETALON_FILES_DIR=\"${CMAKE_CURRENT_LIST_DIR}/EtalonBitmatrixFiles/\"")

target_link_libraries(Test__OutMsk PUBLIC LayoutMatrix BitMatrix Vector2D Base64 RLE Crc32 ByteHandler LayoutConverter LayoutFragment LayoutBitmapZond LayoutReader LayoutProcessor)
