if(${CMAKE_VERSION} VERSION_LESS 3.27.0)
    cmake_minimum_required(VERSION 3.11)
else()
    cmake_minimum_required(VERSION 3.27)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Samples)

# Add Windows Driver Kit (WDK) support for driver samples if we're on Windows
if(WIN32)
    include(FetchContent)

    FetchContent_Declare(
        FindWDK
        GIT_REPOSITORY https://github.com/SergiusTheBest/FindWDK
        GIT_TAG        1e41c97ed8511bd1085eb707e2c357d9f159963f
        GIT_SHALLOW    true
    )
    FetchContent_MakeAvailable(FindWDK)
    list(APPEND CMAKE_MODULE_PATH ${FindWDK_SOURCE_DIR}/cmake)
    find_package(WDK REQUIRED)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wnon-virtual-dtor -Wsuggest-override -Wundef -pedantic -Werror")
    if(CMAKE_COMPILER_IS_CLANGXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnewline-eof")
    endif()
endif()

add_subdirectory(CalcLib)
add_subdirectory(CalcTest)
