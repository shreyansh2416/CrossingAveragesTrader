cmake_minimum_required(VERSION 3.7)
project(SMACrossoverTradingStrategy)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define targets, etc. as usual
add_executable(SMACrossoverTradingStrategy sma_crossover.cpp)
