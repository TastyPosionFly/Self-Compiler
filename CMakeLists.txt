cmake_minimum_required(VERSION 3.27)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "D:\\QT\\6.8.0\\mingw_64\\lib\\cmake")

find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)

add_executable(Lexer
        Lexer/main.cpp
        Lexer/Lexer.cpp
)

add_executable(Parser
        Parser/main.cpp
        Parser/parser.h
        Parser/parser.cpp
        Parser/LRParserGenerator.cpp
        Parser/LRParserGenerator.h
)

add_executable(GUI
        GUI/main.cpp
        GUI/GUI.h
        Lexer/Lexer.cpp
)

target_link_libraries(GUI Qt6::Widgets)