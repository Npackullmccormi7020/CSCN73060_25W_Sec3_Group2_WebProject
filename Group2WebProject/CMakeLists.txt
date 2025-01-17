cmake_minimum_required(VERSION 3.7)

project(Group2WebProject)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Find required packages
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(${Boost_INCLUDE_DIRS})

# Add the executable target
add_executable(Group2WebProject main.cpp)

# Link libraries to the target
target_link_libraries(Group2WebProject PRIVATE ${Boost_LIBRARIES} Threads::Threads SQLite::SQLite3)
