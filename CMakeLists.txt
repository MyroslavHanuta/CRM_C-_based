cmake_minimum_required(VERSION 3.20)
project(Assignment)

set(CMAKE_CXX_STANDARD 20)

# --- FTXUI Setup Start ---
include(FetchContent)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG v5.0.0 # Or use 'main' for latest
)

FetchContent_MakeAvailable(ftxui)
# --- FTXUI Setup End ---

add_executable(Assignment
        main.cpp
        hashing.h
        hashing.cpp
        sqlite3.c
        database_control.h
        database_control.cpp)
target_link_libraries(Assignment PRIVATE
        dl
        pthread
        ftxui::screen
        ftxui::dom
        ftxui::component
        hpdf
)
