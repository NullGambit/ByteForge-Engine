/* ----------------------------------------------------------------------------
Copyright (c) 2018-2025, Microsoft Research, Daan Leijen
This is free software; you can redistribute it and/or modify it under the
terms of the MIT license. A copy of the license can be found in the file
"LICENSE" at the root of this distribution.
-----------------------------------------------------------------------------*/
module mimalloc;

import core.stdc.config;

@nogc:

extern (C):

enum MI_MALLOC_VERSION = 225; // major + 2 digits minor

// ------------------------------------------------------
// Compiler specific attributes
// ------------------------------------------------------

// C++11 // includes clang, icc, and clang-cl // includes clang and icc // leads to warnings... __attribute__((cdecl))

// ------------------------------------------------------
// Includes
// ------------------------------------------------------

// size_t
// bool
// INTPTR_MAX

// ------------------------------------------------------
// Standard malloc interface
// ------------------------------------------------------

void* mi_malloc(size_t size);
void* mi_calloc(size_t count, size_t size);
void* mi_realloc(void* p, size_t newsize);
void* mi_expand(void* p, size_t newsize);

void mi_free(void* p);
char* mi_strdup(const(char)* s);
char* mi_strndup(const(char)* s, size_t n);
char* mi_realpath(const(char)* fname, char* resolved_name);

// ------------------------------------------------------
// Extended functionality
// ------------------------------------------------------
enum MI_SMALL_WSIZE_MAX = 128;
enum MI_SMALL_SIZE_MAX = MI_SMALL_WSIZE_MAX * (void*).sizeof;

void* mi_malloc_small(size_t size);
void* mi_zalloc_small(size_t size);
void* mi_zalloc(size_t size);

void* mi_mallocn(size_t count, size_t size);
void* mi_reallocn(void* p, size_t count, size_t size);
void* mi_reallocf(void* p, size_t newsize);

size_t mi_usable_size(const(void)* p);
size_t mi_good_size(size_t size);

// ------------------------------------------------------
// Internals
// ------------------------------------------------------

alias mi_deferred_free_fun = void function(bool force, ulong heartbeat, void* arg);
void mi_register_deferred_free(void function() deferred_free, void* arg);

alias mi_output_fun = void function(const(char)* msg, void* arg);
void mi_register_output(void function() out_, void* arg);

alias mi_error_fun = void function(int err, void* arg);
void mi_register_error(void function() fun, void* arg);

void mi_collect(bool force);
int mi_version();
void mi_stats_reset();
void mi_stats_merge();
void mi_stats_print(void* out_); // backward compatibility: `out` is ignored and should be NULL
void mi_stats_print_out(void function() out_, void* arg);
void mi_thread_stats_print_out(void function() out_, void* arg);
void mi_options_print();

void mi_process_info(
    size_t* elapsed_msecs,
    size_t* user_msecs,
    size_t* system_msecs,
    size_t* current_rss,
    size_t* peak_rss,
    size_t* current_commit,
    size_t* peak_commit,
    size_t* page_faults);

// Generally do not use the following as these are usually called automatically
void mi_process_init();
void mi_process_done();
void mi_thread_init();
void mi_thread_done();

// -------------------------------------------------------------------------------------
// Aligned allocation
// Note that `alignment` always follows `size` for consistency with unaligned
// allocation, but unfortunately this differs from `posix_memalign` and `aligned_alloc`.
// -------------------------------------------------------------------------------------

void* mi_malloc_aligned(size_t size, size_t alignment);
void* mi_malloc_aligned_at(size_t size, size_t alignment, size_t offset);
void* mi_zalloc_aligned(size_t size, size_t alignment);
void* mi_zalloc_aligned_at(size_t size, size_t alignment, size_t offset);
void* mi_calloc_aligned(size_t count, size_t size, size_t alignment);
void* mi_calloc_aligned_at(size_t count, size_t size, size_t alignment, size_t offset);
void* mi_realloc_aligned(void* p, size_t newsize, size_t alignment);
void* mi_realloc_aligned_at(void* p, size_t newsize, size_t alignment, size_t offset);

// -------------------------------------------------------------------------------------
// Heaps: first-class, but can only allocate from the same thread that created it.
// -------------------------------------------------------------------------------------

struct mi_heap_s;
alias mi_heap_t = mi_heap_s;

mi_heap_t* mi_heap_new();
void mi_heap_delete(mi_heap_t* heap);
void mi_heap_destroy(mi_heap_t* heap);
mi_heap_t* mi_heap_set_default(mi_heap_t* heap);
mi_heap_t* mi_heap_get_default();
mi_heap_t* mi_heap_get_backing();
void mi_heap_collect(mi_heap_t* heap, bool force);

void* mi_heap_malloc(mi_heap_t* heap, size_t size);
void* mi_heap_zalloc(mi_heap_t* heap, size_t size);
void* mi_heap_calloc(mi_heap_t* heap, size_t count, size_t size);
void* mi_heap_mallocn(mi_heap_t* heap, size_t count, size_t size);
void* mi_heap_malloc_small(mi_heap_t* heap, size_t size);

void* mi_heap_realloc(mi_heap_t* heap, void* p, size_t newsize);
void* mi_heap_reallocn(mi_heap_t* heap, void* p, size_t count, size_t size);
void* mi_heap_reallocf(mi_heap_t* heap, void* p, size_t newsize);

char* mi_heap_strdup(mi_heap_t* heap, const(char)* s);
char* mi_heap_strndup(mi_heap_t* heap, const(char)* s, size_t n);
char* mi_heap_realpath(mi_heap_t* heap, const(char)* fname, char* resolved_name);

void* mi_heap_malloc_aligned(mi_heap_t* heap, size_t size, size_t alignment);
void* mi_heap_malloc_aligned_at(mi_heap_t* heap, size_t size, size_t alignment, size_t offset);
void* mi_heap_zalloc_aligned(mi_heap_t* heap, size_t size, size_t alignment);
void* mi_heap_zalloc_aligned_at(mi_heap_t* heap, size_t size, size_t alignment, size_t offset);
void* mi_heap_calloc_aligned(mi_heap_t* heap, size_t count, size_t size, size_t alignment);
void* mi_heap_calloc_aligned_at(mi_heap_t* heap, size_t count, size_t size, size_t alignment, size_t offset);
void* mi_heap_realloc_aligned(mi_heap_t* heap, void* p, size_t newsize, size_t alignment);
void* mi_heap_realloc_aligned_at(mi_heap_t* heap, void* p, size_t newsize, size_t alignment, size_t offset);

// --------------------------------------------------------------------------------
// Zero initialized re-allocation.
// Only valid on memory that was originally allocated with zero initialization too.
// e.g. `mi_calloc`, `mi_zalloc`, `mi_zalloc_aligned` etc.
// see <https://github.com/microsoft/mimalloc/issues/63#issuecomment-508272992>
// --------------------------------------------------------------------------------

void* mi_rezalloc(void* p, size_t newsize);
void* mi_recalloc(void* p, size_t newcount, size_t size);

void* mi_rezalloc_aligned(void* p, size_t newsize, size_t alignment);
void* mi_rezalloc_aligned_at(void* p, size_t newsize, size_t alignment, size_t offset);
void* mi_recalloc_aligned(void* p, size_t newcount, size_t size, size_t alignment);
void* mi_recalloc_aligned_at(void* p, size_t newcount, size_t size, size_t alignment, size_t offset);

void* mi_heap_rezalloc(mi_heap_t* heap, void* p, size_t newsize);
void* mi_heap_recalloc(mi_heap_t* heap, void* p, size_t newcount, size_t size);

void* mi_heap_rezalloc_aligned(mi_heap_t* heap, void* p, size_t newsize, size_t alignment);
void* mi_heap_rezalloc_aligned_at(mi_heap_t* heap, void* p, size_t newsize, size_t alignment, size_t offset);
void* mi_heap_recalloc_aligned(mi_heap_t* heap, void* p, size_t newcount, size_t size, size_t alignment);
void* mi_heap_recalloc_aligned_at(mi_heap_t* heap, void* p, size_t newcount, size_t size, size_t alignment, size_t offset);

// ------------------------------------------------------
// Analysis
// ------------------------------------------------------

bool mi_heap_contains_block(mi_heap_t* heap, const(void)* p);
bool mi_heap_check_owned(mi_heap_t* heap, const(void)* p);
bool mi_check_owned(const(void)* p);

// An area of heap space contains blocks of a single size.
struct mi_heap_area_s
{
    void* blocks; // start of the area containing heap blocks
    size_t reserved; // bytes reserved for this area (virtual)
    size_t committed; // current available bytes for this area
    size_t used; // number of allocated blocks
    size_t block_size; // size in bytes of each block
    size_t full_block_size; // size in bytes of a full block including padding and metadata.
    int heap_tag; // heap tag associated with this area
}

alias mi_heap_area_t = mi_heap_area_s;

alias mi_block_visit_fun = bool function(const(mi_heap_t)* heap, const(mi_heap_area_t)* area, void* block, size_t block_size, void* arg);

bool mi_heap_visit_blocks(const(mi_heap_t)* heap, bool visit_blocks, bool function() visitor, void* arg);

// Experimental
bool mi_is_in_heap_region(const(void)* p);
bool mi_is_redirected();

int mi_reserve_huge_os_pages_interleave(size_t pages, size_t numa_nodes, size_t timeout_msecs);
int mi_reserve_huge_os_pages_at(size_t pages, int numa_node, size_t timeout_msecs);

int mi_reserve_os_memory(size_t size, bool commit, bool allow_large);
bool mi_manage_os_memory(void* start, size_t size, bool is_committed, bool is_large, bool is_zero, int numa_node);

void mi_debug_show_arenas();
void mi_arenas_print();

// Experimental: heaps associated with specific memory arena's
alias mi_arena_id_t = int;
void* mi_arena_area(mi_arena_id_t arena_id, size_t* size);
int mi_reserve_huge_os_pages_at_ex(size_t pages, int numa_node, size_t timeout_msecs, bool exclusive, mi_arena_id_t* arena_id);
int mi_reserve_os_memory_ex(size_t size, bool commit, bool allow_large, bool exclusive, mi_arena_id_t* arena_id);
bool mi_manage_os_memory_ex(void* start, size_t size, bool is_committed, bool is_large, bool is_zero, int numa_node, bool exclusive, mi_arena_id_t* arena_id);

// Create a heap that only allocates in the specified arena
mi_heap_t* mi_heap_new_in_arena(mi_arena_id_t arena_id);

// Experimental: allow sub-processes whose memory areas stay separated (and no reclamation between them)
// Used for example for separate interpreters in one process.
alias mi_subproc_id_t = void*;
mi_subproc_id_t mi_subproc_main();
mi_subproc_id_t mi_subproc_new();
void mi_subproc_delete(mi_subproc_id_t subproc);
void mi_subproc_add_current_thread(mi_subproc_id_t subproc); // this should be called right after a thread is created (and no allocation has taken place yet)

// Experimental: visit abandoned heap areas (that are not owned by a specific heap)
bool mi_abandoned_visit_blocks(mi_subproc_id_t subproc_id, int heap_tag, bool visit_blocks, bool function() visitor, void* arg);

// Experimental: objects followed by a guard page.
// A sample rate of 0 disables guarded objects, while 1 uses a guard page for every object.
// A seed of 0 uses a random start point. Only objects within the size bound are eligable for guard pages.
void mi_heap_guarded_set_sample_rate(mi_heap_t* heap, size_t sample_rate, size_t seed);
void mi_heap_guarded_set_size_bound(mi_heap_t* heap, size_t min, size_t max);

// Experimental: communicate that the thread is part of a threadpool
void mi_thread_set_in_threadpool();

// Experimental: create a new heap with a specified heap tag. Set `allow_destroy` to false to allow the thread
// to reclaim abandoned memory (with a compatible heap_tag and arena_id) but in that case `mi_heap_destroy` will
// fall back to `mi_heap_delete`.
mi_heap_t* mi_heap_new_ex(int heap_tag, bool allow_destroy, mi_arena_id_t arena_id);

// deprecated
int mi_reserve_huge_os_pages(size_t pages, double max_secs, size_t* pages_reserved);
void mi_collect_reduce(size_t target_thread_owned);

// ------------------------------------------------------
// Convenience
// ------------------------------------------------------

// ------------------------------------------------------
// Options
// ------------------------------------------------------

enum mi_option_e
{
    // stable options
    mi_option_show_errors = 0, // print error messages
    mi_option_show_stats = 1, // print statistics on termination
    mi_option_verbose = 2, // print verbose messages
    // advanced options
    mi_option_eager_commit = 3, // eager commit segments? (after `eager_commit_delay` segments) (=1)
    mi_option_arena_eager_commit = 4, // eager commit arenas? Use 2 to enable just on overcommit systems (=2)
    mi_option_purge_decommits = 5, // should a memory purge decommit? (=1). Set to 0 to use memory reset on a purge (instead of decommit)
    mi_option_allow_large_os_pages = 6, // allow large (2 or 4 MiB) OS pages, implies eager commit. If false, also disables THP for the process.
    mi_option_reserve_huge_os_pages = 7, // reserve N huge OS pages (1GiB pages) at startup
    mi_option_reserve_huge_os_pages_at = 8, // reserve huge OS pages at a specific NUMA node
    mi_option_reserve_os_memory = 9, // reserve specified amount of OS memory in an arena at startup (internally, this value is in KiB; use `mi_option_get_size`)
    mi_option_deprecated_segment_cache = 10,
    mi_option_deprecated_page_reset = 11,
    mi_option_abandoned_page_purge = 12, // immediately purge delayed purges on thread termination
    mi_option_deprecated_segment_reset = 13,
    mi_option_eager_commit_delay = 14, // the first N segments per thread are not eagerly committed (but per page in the segment on demand)
    mi_option_purge_delay = 15, // memory purging is delayed by N milli seconds; use 0 for immediate purging or -1 for no purging at all. (=10)
    mi_option_use_numa_nodes = 16, // 0 = use all available numa nodes, otherwise use at most N nodes.
    mi_option_disallow_os_alloc = 17, // 1 = do not use OS memory for allocation (but only programmatically reserved arenas)
    mi_option_os_tag = 18, // tag used for OS logging (macOS only for now) (=100)
    mi_option_max_errors = 19, // issue at most N error messages
    mi_option_max_warnings = 20, // issue at most N warning messages
    mi_option_max_segment_reclaim = 21, // max. percentage of the abandoned segments can be reclaimed per try (=10%)
    mi_option_destroy_on_exit = 22, // if set, release all memory on exit; sometimes used for dynamic unloading but can be unsafe
    mi_option_arena_reserve = 23, // initial memory size for arena reservation (= 1 GiB on 64-bit) (internally, this value is in KiB; use `mi_option_get_size`)
    mi_option_arena_purge_mult = 24, // multiplier for `purge_delay` for the purging delay for arenas (=10)
    mi_option_purge_extend_delay = 25,
    mi_option_abandoned_reclaim_on_free = 26, // allow to reclaim an abandoned segment on a free (=1)
    mi_option_disallow_arena_alloc = 27, // 1 = do not use arena's for allocation (except if using specific arena id's)
    mi_option_retry_on_oom = 28, // retry on out-of-memory for N milli seconds (=400), set to 0 to disable retries. (only on windows)
    mi_option_visit_abandoned = 29, // allow visiting heap blocks from abandoned threads (=0)
    mi_option_guarded_min = 30, // only used when building with MI_GUARDED: minimal rounded object size for guarded objects (=0)
    mi_option_guarded_max = 31, // only used when building with MI_GUARDED: maximal rounded object size for guarded objects (=0)
    mi_option_guarded_precise = 32, // disregard minimal alignment requirement to always place guarded blocks exactly in front of a guard page (=0)
    mi_option_guarded_sample_rate = 33, // 1 out of N allocations in the min/max range will be guarded (=1000)
    mi_option_guarded_sample_seed = 34, // can be set to allow for a (more) deterministic re-execution when a guard page is triggered (=0)
    mi_option_target_segments_per_thread = 35, // experimental (=0)
    mi_option_generic_collect = 36, // collect heaps every N (=10000) generic allocation calls
    _mi_option_last = 37,
    // legacy option names
    mi_option_large_os_pages = mi_option_allow_large_os_pages,
    mi_option_eager_region_commit = mi_option_arena_eager_commit,
    mi_option_reset_decommits = mi_option_purge_decommits,
    mi_option_reset_delay = mi_option_purge_delay,
    mi_option_abandoned_page_reset = mi_option_abandoned_page_purge,
    mi_option_limit_os_alloc = mi_option_disallow_os_alloc
}

alias mi_option_t = mi_option_e;

bool mi_option_is_enabled(mi_option_t option);
void mi_option_enable(mi_option_t option);
void mi_option_disable(mi_option_t option);
void mi_option_set_enabled(mi_option_t option, bool enable);
void mi_option_set_enabled_default(mi_option_t option, bool enable);

c_long mi_option_get(mi_option_t option);
c_long mi_option_get_clamp(mi_option_t option, c_long min, c_long max);
size_t mi_option_get_size(mi_option_t option);
void mi_option_set(mi_option_t option, c_long value);
void mi_option_set_default(mi_option_t option, c_long value);

// -------------------------------------------------------------------------------------------------------
// "mi" prefixed implementations of various posix, Unix, Windows, and C++ allocation functions.
// (This can be convenient when providing overrides of these functions as done in `mimalloc-override.h`.)
// note: we use `mi_cfree` as "checked free" and it checks if the pointer is in our heap before free-ing.
// -------------------------------------------------------------------------------------------------------

void mi_cfree(void* p);
void* mi__expand(void* p, size_t newsize);
size_t mi_malloc_size(const(void)* p);
size_t mi_malloc_good_size(size_t size);
size_t mi_malloc_usable_size(const(void)* p);

int mi_posix_memalign(void** p, size_t alignment, size_t size);
void* mi_memalign(size_t alignment, size_t size);
void* mi_valloc(size_t size);
void* mi_pvalloc(size_t size);
void* mi_aligned_alloc(size_t alignment, size_t size);

void* mi_reallocarray(void* p, size_t count, size_t size);
int mi_reallocarr(void* p, size_t count, size_t size);
void* mi_aligned_recalloc(void* p, size_t newcount, size_t size, size_t alignment);
void* mi_aligned_offset_recalloc(void* p, size_t newcount, size_t size, size_t alignment, size_t offset);

ushort* mi_wcsdup(const(ushort)* s);
ubyte* mi_mbsdup(const(ubyte)* s);
int mi_dupenv_s(char** buf, size_t* size, const(char)* name);
int mi_wdupenv_s(ushort** buf, size_t* size, const(ushort)* name);

void mi_free_size(void* p, size_t size);
void mi_free_size_aligned(void* p, size_t size, size_t alignment);
void mi_free_aligned(void* p, size_t alignment);

// The `mi_new` wrappers implement C++ semantics on out-of-memory instead of directly returning `NULL`.
// (and call `std::get_new_handler` and potentially raise a `std::bad_alloc` exception).
void* mi_new(size_t size);
void* mi_new_aligned(size_t size, size_t alignment);
void* mi_new_nothrow(size_t size);
void* mi_new_aligned_nothrow(size_t size, size_t alignment);
void* mi_new_n(size_t count, size_t size);
void* mi_new_realloc(void* p, size_t newsize);
void* mi_new_reallocn(void* p, size_t newcount, size_t size);

void* mi_heap_alloc_new(mi_heap_t* heap, size_t size);
void* mi_heap_alloc_new_n(mi_heap_t* heap, size_t count, size_t size);

// ---------------------------------------------------------------------------------------------
// Implement the C++ std::allocator interface for use in STL containers.
// (note: see `mimalloc-new-delete.h` for overriding the new/delete operators globally)
// ---------------------------------------------------------------------------------------------

// std::size_t
// PTRDIFF_MAX
// C++11
// std::true_type
// std::forward

// C++11

// C++17

// C++11

// C++11

// std::shared_ptr

// Common base class for STL allocators in a specific heap

/* will not delete nor destroy the passed in heap */

// C++17

// C++11

/* calls heap_delete/destroy when the refcount drops to zero */

// STL allocator allocation in a specific heap

// creates fresh heap that is deleted when the destructor is called
// no delete nor destroy on the passed in heap

// STL allocator allocation in a specific heap, where `free` does nothing and
// the heap is destroyed in one go on destruction -- use with care!

// creates fresh heap that is destroyed when the destructor is called
// no delete nor destroy on the passed in heap

/* do nothing as we destroy the heap on destruct. */

// C++11

// __cplusplus
