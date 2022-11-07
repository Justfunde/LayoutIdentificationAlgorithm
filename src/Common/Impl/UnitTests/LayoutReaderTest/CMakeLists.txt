set(SOURCES_LIST LayoutReaderTest.cpp)

set(LIBS_LIST LayoutReader)

add_unit_test(
   NAME Test__MskReader
   SOURCES ${SOURCES_LIST}
   LIBRARIES ${LIBS_LIST}
   DEFINITIONS "TEST_FILES_DIR=\"${CMAKE_CURRENT_LIST_DIR}/MskFiles/\"")
