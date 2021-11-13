/*
   SPDX-License-Identifier: Unlicense

   DO NOT USE. These are non-functional stubs.

*/

#include <cstdint>
#include <atomic>
#include <algorithm>

extern "C" std::uintptr_t metal_segment_heap_target_start ;
extern "C" std::uintptr_t metal_segment_heap_target_end  ;

namespace {
    volatile std::atomic<std::size_t> heap_offset = 0;

};

void* operator new(std::size_t size) {
    std::size_t start_offset = heap_offset.fetch_add(size, std::memory_order_relaxed);
    return reinterpret_cast<void*>(metal_segment_heap_target_start + start_offset);
}
void operator delete(void*) {
    // Delete does nothing..
}

extern "C" void *memcpy(void *dst, const void * src, std::size_t n) {
    // Assumes src and dst do not overlap..
    uint8_t *dst_data = reinterpret_cast<uint8_t*>(dst);
    const uint8_t *src_data = reinterpret_cast<const uint8_t*>(src);
    for (auto i = n; i>0; i--) {
        *dst_data++ = *src_data++;
    }
    return reinterpret_cast<void*>(dst_data);
}


