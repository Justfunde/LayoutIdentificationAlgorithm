macro(add_unit_test)

      set(oneValueArgs NAME)
      set(multiValueArgs LIBRARIES DEFINITIONS SOURCES)

      cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

      add_executable(${TEST_NAME} ${TEST_SOURCES})

      target_compile_definitions(${TEST_NAME} PRIVATE ${TEST_DEFINITIONS})
      
      target_link_libraries(${TEST_NAME} PRIVATE ${TEST_LIBRARIES})
      
      target_include_directories(${TEST_NAME} PRIVATE ${SHARED_INCLUDE_DIRS})
      
      set_target_properties(${TEST_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/BuiltTests/Debug"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/BuiltTests/Release")

      add_test(NAME ${TEST_NAME} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/BuiltTests/${CMAKE_BUILD_TYPE} COMMAND ${TEST_NAME})

endmacro(add_unit_test)