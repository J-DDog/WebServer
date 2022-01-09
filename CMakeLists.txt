cmake_minimum_required(VERSION "3.10")

project(JDogsWebServer)

add_executable(JDogsWebServer ./src/main.cpp)

set(Libraries
    "ws2_32"

)

target_link_libraries(JDogsWebServer ${Libraries})