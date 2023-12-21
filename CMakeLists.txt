cmake_minimum_required(VERSION 3.27)
project(neural-network)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-O3")

add_subdirectory(external/FTXUI)
add_executable(nn src/main.cpp)
target_compile_features(nn PRIVATE cxx_std_20)
target_include_directories(nn PRIVATE external/FTXUI/include)
target_link_libraries(nn
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
