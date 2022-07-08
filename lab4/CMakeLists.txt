cmake_minimum_required(VERSION 3.5)

project(lab3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC "src/*.cpp"
    "src/task1/*.cpp"
    "src/task3/*.cpp"
    "src/CoGen/*.cpp"
)
add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE src)
