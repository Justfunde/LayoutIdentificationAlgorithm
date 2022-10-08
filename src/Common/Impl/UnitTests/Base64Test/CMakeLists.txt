set(SOURCES_LIST Base64Test.cpp)

set(LIBS_LIST Base64)

add_unit_test(
   NAME Test__Base64
   SOURCES ${SOURCES_LIST}
   LIBRARIES ${LIBS_LIST})