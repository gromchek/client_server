cmake_minimum_required(VERSION 3.10)

project(helloworld)

add_executable(client client.cpp)
add_executable(server server.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(client PRIVATE Threads::Threads)
target_link_libraries(server PRIVATE Threads::Threads)

set_target_properties(server client PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(client PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_options(server PRIVATE -Wall -Wextra -pedantic -Werror)

