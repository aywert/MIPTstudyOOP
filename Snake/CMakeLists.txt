cmake_minimum_required(VERSION 3.16)
project(game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML REQUIRED sfml-graphics sfml-window sfml-system)

file(GLOB SRC_CPP "src/*.cpp" "src/view/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_CPP})

# 5. Настраиваем пути и линковку
target_include_directories(${PROJECT_NAME} PRIVATE
  include
  include/view
)

target_link_libraries(${PROJECT_NAME}
  sfml-graphics 
  sfml-window 
  sfml-system
)