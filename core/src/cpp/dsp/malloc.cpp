//
// Created by Elec332 on 13/11/2021.
//

#include <nativesdr/dsp/malloc.h>
#include <volk/volk.h>

static size_t alignment = 16;

void init_malloc() {
    alignment = volk_get_alignment();
}

void* dsp::malloc(size_t length) {
    return volk_malloc(length, alignment);
}

void dsp::free(void* ptr) {
    volk_free(ptr);
}