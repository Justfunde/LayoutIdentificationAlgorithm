set(SOURCES_LIST ByteHandlerTest.cpp)

set(LIBS_LIST ByteHandler)

add_unit_test(
   NAME Test__ByteHandler
   SOURCES ${SOURCES_LIST}
   LIBRARIES ${LIBS_LIST})
