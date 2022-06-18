cmake_minimum_required(VERSION 3.20.2)
project(travel-codigo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include_directories(include)
add_executable(travel
    main.cpp
    src/entities/graph.cpp
    src/dataset.cpp
    src/ui.cpp
    src/interact.cpp
)
