cmake_minimum_required(VERSION 3.12)
project(hundred-operator-fm)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif ()

if (APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-framework CoreAudio")
endif ()

include_directories(include)

add_executable(hundred-operator hundred-operator.cpp)
