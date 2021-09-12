cmake_minimum_required(VERSION 3.16)

#use the below command to sync clocks
#sudo ntpdate time.windows.com

project(QLWrapperLib
        VERSION 0.1.0
        DESCRIPTION "A C++ wrapper library that calls QuantLib"
        LANGUAGES CXX
        )

## Require C++11 for Catch2
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(src)

#if(TESTS)
    enable_testing()
    add_subdirectory(tests)
#endif()