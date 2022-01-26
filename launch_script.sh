#!/bin/bash

rm *gch; clear;
clang++ -Wall -Wextra -Werror -std=c++98 -fsanitize=address main.cpp request_parser/*.cpp utils/*.cpp response/*cpp cgiHandler/*.cpp config_parser/*.cpp -I config_parser/Parser.hpp
