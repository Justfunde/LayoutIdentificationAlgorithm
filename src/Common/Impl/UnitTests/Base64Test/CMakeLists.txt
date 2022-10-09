set(SOURCES_LIST Base64Test.cpp)

set(LIBS_LIST Base64 Crc32)

add_unit_test(
   NAME Test__Base64Standard_Fuzzing
   SOURCES ${SOURCES_LIST}
   LIBRARIES ${LIBS_LIST})