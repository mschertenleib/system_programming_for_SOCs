#include <stdio.h>
#include <perf.h>
#include <swap.h>
#include <cache.h>
#include <spr.h>
#include "support/include/exception.h"

exception_handler_t my_syscall_exception(void){
    uint32_t epc = SPR_READ(SPR_EPC);
    printf("syscall EPC : 0x%08x",epc);
}

int main () {
   /* enable the caches */
   icache_write_cfg( CACHE_DIRECT_MAPPED | CACHE_SIZE_2K | CACHE_REPLACE_FIFO );
   dcache_write_cfg( CACHE_FOUR_WAY | CACHE_SIZE_2K | CACHE_REPLACE_LRU | CACHE_WRITE_BACK );
   icache_enable(1);
   dcache_enable(1);
   /* initialize the performance counters */
   perf_init();

   printf("Hello, world from %s (%s:%2d)!\n", __func__, __FILE__, __LINE__);

   printf("no swap: 0x%04x, swap_u16: 0x%04x\n", 0xDEAD, swap_u16(0xDEAD));
   printf("no swap: 0x%08x, swap_u32: 0x%08x\n", 0xDEADBEEF, swap_u32(0xDEADBEEF));

   perf_start();
   while (1) {
       perf_print_time(PERF_COUNTER_RUNTIME, "runtime");
       perf_print_cycles(PERF_COUNTER_RUNTIME, "runtime");
       for (volatile int i = 0; i < 1000000; ++i) ;

       int *addr = (int*) 0x0000;
       int data;
       asm volatile ("l.lwz %[d], 0(%[s])":[d]"=r"(data):[s]"r"(addr));

       SYSCALL(0xAA);
   }
   perf_stop();

   return 0;
}
