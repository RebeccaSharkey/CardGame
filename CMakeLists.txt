cmake_minimum_required(VERSION 3.20)
project(CardGame)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include engine and game subdirs
add_subdirectory(Engine)
add_subdirectory(Game)