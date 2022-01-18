#!/bin/bash

rm *gch ../utils/*.gch; clear;
clang++ -Wall -Wextra -Werror -std=c++98 *.cpp ../utils/utils.*
