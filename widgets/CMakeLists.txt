include_directories(SYSTEM
                    ${Qt5Widgets_INCLUDE_DIRS}
                    ${Qt5Sql_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Help_INCLUDE_DIRS}
                    ${OPENSSL_INCLUDE_DIR}
                    )

include_directories(${PROJECT_BINARY_DIR}
                    ${PROJECT_SOURCE_DIR}
                    )

file (GLOB_RECURSE SOURCES *.cpp)
file (GLOB_RECURSE HEADERS *.h)

add_library(widgets STATIC ${SOURCES} ${HEADERS})

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
set_source_files_properties("widgets_autogen/mocs_compilation.cpp"
                            PROPERTIES
                            COMPILE_FLAGS
                            "-Wno-useless-cast")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties("widgets_autogen/mocs_compilation.cpp"
                                PROPERTIES
                                COMPILE_FLAGS
                                "-Wno-extra-semi-stmt -Wno-redundant-parens")
endif ()
