include(FetchContent)

set(FLEXLEXER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

# Don't re-download external dependencies if they are already present
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

#------------------------------------------------------------------------------------
# Fetch rang
#------------------------------------------------------------------------------------

message(STATUS "Fetching rang...")
FetchContent_Declare(
        rang
        GIT_REPOSITORY https://github.com/agauniyal/rang
        GIT_TAG 09f2f05bea29d61b1df99e4b36f63d961156a580 # 3.2
)
FetchContent_MakeAvailable(rang)

#------------------------------------------------------------------------------------
# Fetch doctest
#------------------------------------------------------------------------------------

message(STATUS "Fetching doctest...")
set(DOCTEST_WITH_TESTS OFF)
set(DOCTEST_NO_INSTALL ON)
set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF)
FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/onqtam/doctest
        GIT_TAG 4d8716f1efc1d14aa736ef52ee727bd4204f4c40 # 2.4.6
)
FetchContent_MakeAvailable(doctest)
