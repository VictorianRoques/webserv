#!/bin/bash

rm *gch; clear;
clang++ -Wall -Wextra -Werror -std=c++98 *.cpp ../request_parser/*.cpp ../utils/*.cpp ../response/*cpp ../cgiHandler/*.cpp -I Parser.hpp
