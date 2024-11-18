#include <cache.h>
#include <node.h>
#include <perf.h>
#include <platform.h>
#include <stdio.h>
#include <string.h>

#include <task1.h>
#include <task2.h>
#include <task3.h>
#include <task4.h>

int main()
{
    // initializes the UART, performance counters, peripherals etc.
    platform_init();

    icache_write_cfg(CACHE_DIRECT_MAPPED | CACHE_SIZE_8K | CACHE_REPLACE_FIFO);
    dcache_write_cfg(CACHE_FOUR_WAY | CACHE_SIZE_4K | CACHE_REPLACE_LRU | CACHE_WRITE_BACK);
    icache_enable(1);
    dcache_enable(1);

    perf_init();
    perf_set_mask(PERF_COUNTER_0, PERF_DCACHE_MISS_MASK);
    perf_set_mask(PERF_COUNTER_1, PERF_STALL_CYCLES_MASK | PERF_ICACHE_NOP_INSERTION_MASK);
    perf_set_mask(PERF_COUNTER_2, PERF_BUS_IDLE_MASK);
    perf_set_mask(PERF_COUNTER_3, PERF_ICACHE_MISS_MASK);

    // task1_main();
    // task2_main();
    task3_main();
    // task4_main();

    dcache_flush();

    return 0;
}