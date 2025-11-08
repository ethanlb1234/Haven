include(FetchContent)

message(STATUS "Setting up Vulkan Headers")
FetchContent_Declare(
    Vulkan
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG v1.3.280 # Modern version with CMake 3.5+ support
    GIT_PROGRESS TRUE
)

set(CMAKE_WARN_DEPRECATED 0 CACHE INTERNAL "Suppress deprecation shortly")
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
FetchContent_MakeAvailable(Vulkan)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 0 CACHE INTERNAL "Re-enable dev warnings")
set(CMAKE_WARN_DEPRECATED 1 CACHE INTERNAL "Restore deprecation notices")
