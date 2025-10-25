
#include <rtthread.h>
#ifdef PKG_USING_CJSON
#include "cJSON.h"
#endif
#include "doom_mem.h"

#ifdef PKG_USING_CJSON
cJSON_Hooks doom_mem_hook =
{
    .malloc_fn = doom_mem_malloc,
    .free_fn = doom_mem_free,
};
#endif

L2_CACHE_RET_BSS_SECT_BEGIN(doom_psram_ret_cache)
    ALIGN(4) static uint8_t doom_psram_heap[0x700000] SECTION(STRINGIFY(.bss.l2_cache_ret_bss_doom_psram_ret_cache));
L2_CACHE_RET_BSS_SECT_END

static struct rt_memheap doom_memheap;
static struct rt_memheap *p_doom_memheap;

static int doom_memheap_init(void)
{
    rt_memheap_init(&doom_memheap, "doom_memheap", (void *)doom_psram_heap, sizeof(doom_psram_heap));
    p_doom_memheap = &doom_memheap;
#ifdef PKG_USING_CJSON
    cJSON_InitHooks(&doom_mem_hook);
#endif
    return 0;
}
INIT_PREV_EXPORT(doom_memheap_init);

void * doom_mem_malloc(size_t size)
{
    void * ptr = NULL;
    ptr = rt_memheap_alloc(p_doom_memheap, size);
    return ptr;
}

void doom_mem_free(void * ptr)
{
    rt_memheap_free(ptr);
}

void * doom_mem_calloc(uint32_t count, uint32_t size)
{
    void * ptr = NULL;
    ptr = rt_memheap_calloc(p_doom_memheap, count, size);
    return ptr;
}
