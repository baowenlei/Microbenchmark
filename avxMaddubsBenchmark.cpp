//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "benchmark/benchmark.h"
#include <stdlib.h>
#include <malloc.h>
#include <iostream>
#include <vector>
#include <x86intrin.h>

#define ITER 1000000
#define SIZE 128
#define OUTSIZE  64


#define BENCHMARK_CAPTURE_NO_ARG(func, test_case_name)     \
  BENCHMARK_PRIVATE_DECLARE(func) =                      \
      (::benchmark::internal::RegisterBenchmarkInternal( \
          new ::benchmark::internal::FunctionBenchmark(  \
              #func "/" #test_case_name,                 \
              [](::benchmark::State& st) { func(st); })))


using namespace std;

void* AlignedMalloc(size_t size, size_t alignment)
{
    return memalign(alignment, size);
}
void AlignedFree(void *ptr)
{
    free(ptr);
}

void avx2_maddubs(int8_t* vec0, uint8_t* vec1, int16_t* out)
{
    auto p0 = _mm256_lddqu_si256((const __m256i*)(vec0 + (0) * 32));
    auto p1 = _mm256_lddqu_si256((const __m256i*)(vec0 + (1) * 32));
    auto p2 = _mm256_lddqu_si256((const __m256i*)(vec0 + (2) * 32));
    auto p3 = _mm256_lddqu_si256((const __m256i*)(vec0 + (3) * 32));

    auto r0 = _mm256_lddqu_si256((const __m256i *)(vec1 + (0) * 32));
    auto r1 = _mm256_lddqu_si256((const __m256i *)(vec1 + (1) * 32));
    auto r2 = _mm256_lddqu_si256((const __m256i *)(vec1 + (2) * 32));
    auto r3 = _mm256_lddqu_si256((const __m256i *)(vec1 + (3) * 32));

    for (int i = 0; i < ITER; i++)
    {
        p0 = _mm256_maddubs_epi16(p0, r0);
        p1 = _mm256_maddubs_epi16(p1, r1);
        p2 = _mm256_maddubs_epi16(p2, r2);
        p3 = _mm256_maddubs_epi16(p3, r3);
    }

    _mm256_store_si256((__m256i*)(out), p0);
    _mm256_store_si256((__m256i*)(out+16), p1);
    _mm256_store_si256((__m256i*)(out+32), p2);
    _mm256_store_si256((__m256i*)(out+48), p3);
}

void avx512_maddubs(int8_t* vec0, uint8_t* vec1, int16_t* out)
{
    auto p0 = _mm512_load_si512((const void*)(vec0 + (0) * 64));
    auto p1 = _mm512_load_si512((const void*)(vec0 + (1) * 64));

    auto r0 = _mm512_load_si512((const void*)(vec1 + (0) * 64));
    auto r1 = _mm512_load_si512((const void*)(vec1 + (1) * 64));

    for (int i = 0; i < ITER; i++)
    {
        p0 = _mm512_maddubs_epi16(p0, r0);
        p1 = _mm512_maddubs_epi16(p1, r1);
    }

    _mm512_store_si512((void*)(out), p0); 
    _mm512_store_si512((void*)(out+32), p1); 
}

void AVX2_benchmark(benchmark::State& state)
{
    int16_t* result = (int16_t*)AlignedMalloc(OUTSIZE * sizeof(int16_t), 64);
    int8_t* vec0 = (int8_t*)AlignedMalloc(SIZE * sizeof(int8_t), 64);
    uint8_t* vec1 = (uint8_t*)AlignedMalloc(SIZE * sizeof(uint8_t), 64);

    for (int i = 0; i < SIZE; i++)
        vec0[i] = 1;
    for (int i = 0; i < SIZE; i++)
        vec1[i] = 1;

    //warm up
    avx2_maddubs(vec0, vec1, result);

    int i = 0;
    for (auto _ : state)
    {
        avx2_maddubs(vec0, vec1, result);
        i += (int)(*result);
    }

    AlignedFree((void*)result);
}


void AVX512_benchmark(benchmark::State& state)
{
    int16_t* result = (int16_t*)AlignedMalloc(OUTSIZE * sizeof(int16_t), 64);
    int8_t* vec0 = (int8_t*)AlignedMalloc(SIZE * sizeof(int8_t), 64);
    uint8_t* vec1 = (uint8_t*)AlignedMalloc(SIZE * sizeof(uint8_t), 64);

    for (int i = 0; i < SIZE; i++)
        vec0[i] = 1;
    for (int i = 0; i < SIZE; i++)
        vec1[i] = 1;

    //warm up
    avx512_maddubs(vec0, vec1, result);

    int i = 0;
    for (auto _ : state)
    {
        avx512_maddubs(vec0, vec1, result);
        i += (int)(*result);
    }

    AlignedFree((void*)result);
}


BENCHMARK_CAPTURE_NO_ARG(AVX2_benchmark,maddubs);
BENCHMARK_CAPTURE_NO_ARG(AVX512_benchmark,maddubs);

BENCHMARK_MAIN();

