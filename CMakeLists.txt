cmake_minimum_required(VERSION 3.16)
project(MapViewer)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent for downloading SFML
include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
)

FetchContent_MakeAvailable(SFML)

# Add the executable
add_executable(MapViewer 
    src/main.cpp
    src/App.cpp
    src/GraphLoader.cpp
    src/ParseOSM.cpp
    src/Binary.cpp
    src/Graph.cpp
    src/Graphics.cpp
    src/Algorithm.cpp
    src/Quadtree.cpp
)

# Include both 'include' and 'external' directories for headers
target_include_directories(MapViewer PRIVATE 
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/external
)

# Make the root directory available in project files
target_compile_definitions(MapViewer PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# Link SFML libraries
target_link_libraries(MapViewer PRIVATE sfml-graphics sfml-window sfml-system)

# Set include directories
target_include_directories(MapViewer PRIVATE ${SFML_SOURCE_DIR}/include)

# Include the 'include' directory for headers
target_include_directories(MapViewer PRIVATE ${CMAKE_SOURCE_DIR}/include)