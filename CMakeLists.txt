cmake_minimum_required(VERSION 3.10)

# set the project name
project(popdb)
set(CMAKE_CXX_STANDARD 20)

#add_compile_definitions(DEBUG_LEXER)
#add_compile_definitions(DEBUG_LOGGING)
# add the executable
#set(CMAKE_VERBOSE_MAKEFILE ON)
set_source_files_properties(lex.pop.c PROPERTIES LANGUAGE CXX )
add_executable(
    popdb main.cpp Lexer.cpp Lexer.hpp
    expression_common.hpp expression_common.cpp expression.cpp expression.hpp Lexer.hpp
    Lexer.cpp Engine.h Engine.cpp EngineException.cpp EngineException.h
    OperatorHandler.cpp OperatorHandler.h Engine_operators.cpp Engine_keywords.cpp
    utils.cpp utils.hpp default_functions.cpp
    value.hpp value.cpp
    fragment.h
    lex.pop.c
    gc.h gc.cpp
)

add_subdirectory(Asago)
target_link_libraries(popdb PRIVATE Asago)