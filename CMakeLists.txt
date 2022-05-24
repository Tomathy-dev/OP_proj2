cmake_minimum_required(VERSION 3.21)
project(untitled4 C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(src)

add_executable(untitled4
        include/client.h
        include/driver.h
        include/main.h
        include/memory-private.h
        include/memory.h
        include/process.h
        include/restaurant.h
        include/utils.h
        include/synchronization.h
        include/configuration.h
        include/log.h
        include/mesignal.h
        include/metime.h
        include/stats.h
        src/client.c
        src/driver.c
        src/main.c
        src/memory-private.c
        src/memory.c
        src/process.c
        src/restaurant.c
        src/utils.c
        src/synchronization.c
        src/configuration.c
        src/log.c
        src/mesignal.c
        src/metime.c
        src/stats.c)

find_library(LIBRT rt)
if(LIBRT)
    target_link_libraries(${PROJECT_NAME} ${LIBRT})
endif()
target_link_libraries(${PROJECT_NAME} m)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)