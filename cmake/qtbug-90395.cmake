cmake_minimum_required(VERSION 3.0)

# QT 5 LTS has missing includes for <limit> and fails with GCC 11
# https://bugreports.qt.io/browse/QTBUG-90395
# Force the use of GCC 10 when QT 5 and GCC >=11 detected
if(
        ${QT_VERSION} VERSION_LESS 5.15.9
        AND
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"
        AND
        ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL 11
    )
    message(AUTHOR_WARNING
        "Detected Qt 5 and ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}\n"
        "Forcing use of GCC 10 (https://bugreports.qt.io/browse/QTBUG-90395)"
    )

    set(CMAKE_CXX_COMPILER "g++-10")
    set(CMAKE_C_COMPILER "gcc-10")
endif()
