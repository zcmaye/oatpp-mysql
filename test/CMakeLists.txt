add_executable(oatpp-mysql-tests
        oatpp-mysql/ql_template/ParserTest.hpp
        oatpp-mysql/ql_template/ParserTest.cpp
        oatpp-mysql/types/NumericTest.hpp
        oatpp-mysql/types/NumericTest.cpp
        oatpp-mysql/tests.cpp
)

set_target_properties(oatpp-mysql-tests PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

target_include_directories(oatpp-mysql-tests
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

if(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_EXTERNAL)
    add_dependencies(oatpp-mysql-tests ${LIB_OATPP_EXTERNAL})
endif()

add_dependencies(oatpp-mysql-tests ${OATPP_THIS_MODULE_NAME})

target_link_oatpp(oatpp-mysql-tests)

target_link_libraries(oatpp-mysql-tests
        PRIVATE ${OATPP_THIS_MODULE_NAME}
)

## TODO link dependencies here (if some)

add_test(oatpp-mysql-tests oatpp-mysql-tests)