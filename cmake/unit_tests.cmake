cmake_minimum_required(VERSION 3.5)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Network)

# Required for the example message structure in test_appendix_B.hpp
if(MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND CMAKE_VERSION VERSION_GREATER 3.20.3)
    # this change happened in CMake 3.20.4
    set(CMAKE_CXX_STANDARD 23) # /std:c++latest - unlocks the non stable cpp20 features. For new 16.11 versions
else()
    set(CMAKE_CXX_STANDARD 20) # /std:c++latest for msvc and -std=c++20 for everyone else.
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a single test binary called OTPLib_Tests containing all discovered unit tests
enable_testing()
set(TEST_TARGET "OTPLib_Tests")

# Look for unit test source files
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "src/test_*.cpp")
file(GLOB_RECURSE TEST_HEADERS CONFIGURE_DEPENDS "src/test_*.hpp")

# Remove directory from discovered test CPP
foreach(TEST_LIST_ITEM ${TEST_SOURCES})
    get_filename_component(TEST_FILENAME ${TEST_LIST_ITEM} NAME_WE)
    list(APPEND TEST_SOURCES_NODIR ${TEST_FILENAME})
endforeach()

# Setup single combined test
create_test_sourcelist(
    TEST_LIST
    ${TEST_TARGET}.cpp
    ${TEST_SOURCES_NODIR}
)

# Create the test binary
add_executable(${TEST_TARGET}
    ${TEST_TARGET}.cpp
    ${TEST_SOURCES}
    ${TEST_HEADERS}
    ${LIBRARY_HEADERS}
    ${LIBRARY_SOURCES}
)
target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Qt
target_link_libraries(${TEST_TARGET} PRIVATE
   Qt${QT_VERSION_MAJOR}::Test
   Qt${QT_VERSION_MAJOR}::Network
)

# OTPLib
target_compile_definitions(${TEST_TARGET} PRIVATE MAKE_OTP_LIB)

get_target_property(OTPLIB_INCLUDE_DIRS OTPLib INCLUDE_DIRECTORIES)
target_include_directories(${TEST_TARGET} PRIVATE ${OTPLIB_INCLUDE_DIRS})

get_target_property(OTPLIB_COMPILE_DEFINES OTPLib COMPILE_DEFINITIONS)
target_compile_definitions(${TEST_TARGET} PRIVATE ${OTPLIB_COMPILE_DEFINES})

# Setup a test for each seperate test source
remove (TEST_LIST ${TEST_TARGET}.cpp)
foreach(TEST ${TEST_LIST})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_TARGET} ${TEST_NAME})
    message("Added test: ${TEST_NAME}")
endforeach()


