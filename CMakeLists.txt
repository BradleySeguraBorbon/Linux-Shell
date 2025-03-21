cmake_minimum_required(VERSION 3.30)
project(I_Proyecto)

set(CMAKE_CXX_STANDARD 20)

add_executable(I_Proyecto main.cpp
        Shell.h
        Shell.cpp
        CommandHandler.h
        CommandHandler.cpp
        History.h
        History.cpp)
