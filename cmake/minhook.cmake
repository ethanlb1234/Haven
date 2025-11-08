include(FetchContent)

set(LIB_NAME "minhook")
message(STATUS "Setting up ${LIB_NAME}")

FetchContent_Declare(
    ${LIB_NAME}
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG        v1.3.3
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(${LIB_NAME})