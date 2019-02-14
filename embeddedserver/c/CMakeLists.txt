SET(FLAGS "-Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -D _GNU_SOURCE -lpthread -lm")

# Default target.
SET(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS}")
file(GLOB APP_C_SRC src/*.c)
file(GLOB APP_C_LIB lib/*.c)
add_executable (internalserver ${APP_C_LIB} ${APP_C_SRC})
set_target_properties(internalserver
    PROPERTIES
    LINKER_LANGUAGE C
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dst/embeddedserver/internalserverapp/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dst/embeddedserver/internalserverapp/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dst/embeddedserver/internalserverapp/bin"
)
target_include_directories(internalserver PUBLIC include)
target_compile_options(internalserver PRIVATE -Wall -g -std=c99 "-D _POSIX_C_SOURCE=200809L" -Werror "-D _GNU_SOURCE" -lpthread -lm)
target_link_libraries(internalserver PRIVATE -Wall -g -std=c99 "-D _POSIX_C_SOURCE=200809L" -Werror "-D _GNU_SOURCE" -lpthread -lm)
file(COPY extras DESTINATION ${CMAKE_BINARY_DIR}/dst/embeddedserver/internalserverapp/bin)

# "tests" target.
add_custom_target(tests)
SET(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS}")
file(GLOB TESTS_C_SRC tests/src/*.c)
file(GLOB TESTS_C_LIB tests/lib/*.c)
file(GLOB UNITY_C_SRC external/Unity/src/*.c)
add_executable (testsapp ${TESTS_C_SRC} ${TESTS_C_LIB} ${UNITY_C_SRC} ${APP_C_LIB})
set_target_properties(testsapp
    PROPERTIES
    EXCLUDE_FROM_DEFAULT_BUILD 1
    EXCLUDE_FROM_ALL 1
    LINKER_LANGUAGE C
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dst/embeddedserver/testsapp/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dst/embeddedserver/testsapp/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dst/embeddedserver/testsapp/bin"
)
target_include_directories(testsapp PUBLIC
    include tests/include external/Unity/src
)
target_compile_options(testsapp PRIVATE -Wall -g -std=c99 "-D _POSIX_C_SOURCE=200809L" -Werror "-D _GNU_SOURCE" -lpthread -lm)
target_link_libraries(testsapp PRIVATE -Wall -g -std=c99 "-D _POSIX_C_SOURCE=200809L" -Werror "-D _GNU_SOURCE" -lpthread -lm)
add_dependencies(tests testsapp)
