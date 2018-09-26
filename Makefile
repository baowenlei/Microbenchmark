#*******************************************************************************************************/
#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#
#*******************************************************************************************************/


# libs
BENCHMARK_PATH=/usr/local/benchmark

# flags
CC=g++
CPPFLAGS=-O3 -std=c++11 -march=broadwell -march=skylake-avx512 -fpermissive -fPIC -Werror -fcheck-new -pthread 
INCLUDE_PATH=-I$(BENCHMARK_PATH)/include 
LIB_PATH=-L$(BENCHMARK_PATH)/build/src/ -lbenchmark  

# final output
all:benchmark

benchmark:avxMaddubsBenchmark.cpp
    $(CC) avxMaddubsBenchmark.cpp -o benchmark $(CPPFLAGS) $(INCLUDE_PATH) $(LIB_PATH) 

clean:
    rm -rf *.o benchmark
