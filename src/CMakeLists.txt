add_executable(${PROJECT_NAME}
        main.cpp)
target_link_libraries(${PROJECT_NAME}
        core)
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (MSVC)
    target_compile_options(${PROJECT_NAME}
            PRIVATE /W4 /WX)
    target_compile_definitions(${PROJECT_NAME}
            PRIVATE _CRT_SECURE_NO_WARNINGS)
else ()
    target_compile_options(${PROJECT_NAME}
            PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()
