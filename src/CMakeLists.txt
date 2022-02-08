if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()

add_executable(${PROJECT_NAME}
        main.cpp)
target_link_libraries(${PROJECT_NAME}
        core)
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (MSVC)
    target_compile_definitions(${PROJECT_NAME}
            PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()
