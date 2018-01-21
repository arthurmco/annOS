#pragma once

/**
 * Bitmap-based physical memory allocator
 *
 * Copyright (C) 2018 Arthur M
 */

/**
 * TODO: The CheckPages() method.
 *       Detect overlapping zones, alert the user and do something sane
 *       Merge contiguous zones with the same type
 */

#include <stdint.h>
#include <stddef.h>

namespace annos {

struct MemoryMap {
    uintptr_t start;  // Memory area start, in bytes
    size_t len;       // Memory area length, in bytes
    int type;         // Memory area type
};

// Physical page size, in bytes
#define PHYS_PAGE_SIZE 4096

typedef uintptr_t phys_t;

enum PMMZoneType
{
    
    /**
     * A common, normal, nothing special zone
     */
    Normal = 1,

    /**
     * MMIO-reserved zones
     * They can't be allocated (via AllocatePhysical(), just mapped
     */
    MMIO = 2,

    /**
     * Low-memory zones
     * Used for devices that can't allocate more than 1MB
     * (This might be rarely used outside of x86)
     */
    LowMemory = 4,

    /**
     * 64-bit zones
     * Only effective in 64-bit architecture (that this kernel doesn't support)
     * Any zone that starts and ends in an address above 0xffffffff (4 GB) is 
     * a 64-bit zone
     */
    Zone64Bit = 8,
    
};
    
/**
 * Physical memory manager zone
 * A zone is a range of contiguous pages with something in common
 * It's inherited from the memory map ranges.
 */
struct PMMZone
{
    phys_t start;    // Starting address
    phys_t first_free_addr; // First free address in the bitmap, for fast allocs
    size_t pagecount;   // Physical page count. 1 page = 4 kb
    
    unsigned type; // Address type

    /**
     * The allocation zone bitmap
     * Each bit here represents one phys page.
     */ 
    char* alloc_bitmap;
};


/**
 * The physical memory manager
 */
class PMM {
private:
    PMMZone* _mmap;
    size_t _mmap_count;

    /**
     * Find the zone that the address 'addr' is from 
     * @return Pointer to that zone, or null if can't find a zone
     */
    PMMZone* FindZone(phys_t addr);

    /**
     * Check if 'count' pages are free in the bitmap 'alloc_bitmap_bitmap',
     * starting from 'page_offset'
     *
     * @return True if we have sufficient free pages, false if not
     */
    bool CheckIfPagesFree(char const* alloc_zones_bitmap,
			  unsigned int page_offset,
			  size_t page_count);

    /**
     * Map pages in bitmap 'alloc_zones_bitmap', starting in
     * page_offset until page_count
     */
    void MapPagesInBitmap(char* alloc_zones_bitmap,
			  unsigned int page_offset,
			  size_t page_count);
	
    
public:
    
/**
 * Starts the physical memory manager
 * @param pmm_pool_start The start of the pool, usually the end of the kernel 
 *                       file in memory
 * @param mmap_addr Pointer to the memory map, as returned by the loader
 * @param mmap_count Count of items in the memory map
 */
    PMM(uintptr_t kernel_start, void* pmm_pool_start,
	MemoryMap* mmap_addr, size_t mmap_count);

    /**
     * Allocates n pages starting from physical address 'addr'
     * 
     * @returns phys_t if it succeeds
     * It will panic if it don't, so no worries :v
     */
    phys_t AllocatePhysical(size_t n = 1,
			    PMMZoneType type = PMMZoneType::Normal);

    /**
     * Check if you can allocate 'n' pages starting from the first free address
     * 
     * @returns -1 if you can't because no memory, 0 if because it's fragmented
     * e.g, because there's someone in there, or 1 if you can
     *
     * TODO: Implement later.
     */
    int CheckPages(size_t n = 1);

    /**
     * Maps 'n' pages starting from address 'addr'
     *
     * This is useful for devices that are mapped to a specific physical address,
     * such as framebuffers.
     *
     * @returns the same address if succeeds, or 0xffffffff if fails 
     * (since it couldn't return this address, because it isn't in a page boundary,
     * it's ok to use it as an error code
     */
    phys_t MapPages(phys_t addr, size_t n = 1);

    /**
     * Unmap 'n' pages starting from 'addr'
     * Can be used with AllocatePhysical or MapPages, because both map pages
     *
     * Return the number of pages unmapped
     */
    size_t UnmapPages(phys_t addr, size_t n = 1);

};

}
