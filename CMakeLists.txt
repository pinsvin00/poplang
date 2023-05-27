cmake_minimum_required(VERSION 3.10)

# set the project name
project(popdb)
set(CMAKE_CXX_STANDARD 17)

#add_compile_definitions(DEBUG_LEXER)
add_compile_definitions(DEBUG_LOGGING)
# add the executable
add_executable(popdb main.cpp Lexer.cpp Lexer.hpp expression_common.hpp expression_common.cpp expression.cpp expression.hpp Lexer.hpp Lexer.cpp Engine.h Engine.cpp EngineException.cpp EngineException.h OperatorHandler.cpp OperatorHandler.h Engine_operators.cpp Engine_keywords.cpp utils.cpp utils.hpp)