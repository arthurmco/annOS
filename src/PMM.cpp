#include <PMM.hpp>
#include <Log.hpp>
#include <libk/panic.h>

using namespace annos;

/**
 * Starts the physical memory manager
 * @param pmm_pool_start The start of the pool, usually the end of the kernel 
 *                       file in memory
 * @param mmap_addr Pointer to the memory map, as returned by the loader
 * @param mmap_count Count of items in the memory map
 */
PMM::PMM(uintptr_t kernel_start, void* pmm_pool_start,
	 MemoryMap* mmap_addr, size_t mmap_count)
{
    uintptr_t kend_addr = (uintptr_t)pmm_pool_start;

    // Lambda to allocate memory from the kernel end
    auto kernel_end_malloc = [](uintptr_t* kend, size_t bytes)
	{
	    auto oldmem = *kend;
	    *kend = (oldmem + bytes);
	    return (void*)oldmem;
	};

    this->_mmap = (PMMZone*)kernel_end_malloc(&kend_addr,
					      sizeof(PMMZone)*mmap_count);

    Log::Write(Info, "pmm", "Memory map (as received by the firmware)");
    for (size_t i = 0; i < mmap_count; i++) {
	auto pagecount = (mmap_addr[i].len / PHYS_PAGE_SIZE);
	
	this->_mmap[i].start = mmap_addr[i].start;
	this->_mmap[i].first_free_addr = mmap_addr[i].start;
	this->_mmap[i].pagecount = pagecount;
	this->_mmap[i].type = mmap_addr[i].type;

	// Allocate the zones. size of zones = (pagecount/8)+1
	// +1 is because of rounding.
	this->_mmap[i].alloc_bitmap = (char*)
	    kernel_end_malloc(&kend_addr, (pagecount/8)+1);

	
	Log::Write(Info, "pmm",
		   "\t%d: start 0x%08x, type %02x, using %d phys pages, "
		   "bitmap at 0x%08x",
		   i+1, this->_mmap[i].start, this->_mmap[i].type, 
		   this->_mmap[i].pagecount,
		   (uintptr_t)this->_mmap[i].alloc_bitmap);
    }

    Log::Write(Info, "pmm", "kernel end is at 0x%x, with %d bytes",
	       kend_addr, (kend_addr - (phys_t)pmm_pool_start));

    Log::Write(Debug, "pmm", "mapping the addresses used until now");
    unsigned pmm_page_count = 1 + ((kend_addr - kernel_start) / PHYS_PAGE_SIZE);

    Log::Write(Debug, "pmm", "mapping %d pages for it", pmm_page_count);

    this->_mmap_count = mmap_count;
    if (this->MapPages(kernel_start, pmm_page_count) == ((uint32_t)-1)) {
	Log::Write(Error, "pmm", "no memory to create the tables");
	panic("pmm: no sufficient memory to even create the tables!");
	
    }
}

/**
 * Allocates n pages starting from physical address 'addr'
 * 
 * @returns phys_t if it succeeds
 * It will panic if it don't, so no worries :v
 */
phys_t PMM::AllocatePhysical(size_t n)
{
    auto addr_type = 1;

    // Check for the first non-full memory zone
    for (unsigned i = 0; i < this->_mmap_count; i++) {
	
	if (!(_mmap[i].type & addr_type)) {
	    continue; // Not the desired type. Continue
	}
	
	if (_mmap[i].first_free_addr >=
	    (_mmap[i].start + (_mmap[i].pagecount * PHYS_PAGE_SIZE))) {
	    // Full. Continue
	    continue;
	}

	PMMZone* zone = &_mmap[i];
	/* Advance the actual page until we find sufficient free pages */

	assert(zone->first_free_addr >= zone->start);
	unsigned page_offset = (zone->first_free_addr - zone->start) / PHYS_PAGE_SIZE;

	while (!this->CheckIfPagesFree(zone->alloc_bitmap, page_offset, n)) {
	    page_offset++;

	    if (page_offset >= zone->pagecount) {
		Log::Write(Warning, "pmm", "AllocatePhysical: "
			   "exhausted mmap zone #%d", i);
		continue;
	    }
		
	}

	this->MapPagesInBitmap(zone->alloc_bitmap, page_offset, n);
	phys_t naddr = zone->start + (page_offset * PHYS_PAGE_SIZE);
	phys_t new_first_free = naddr + (n * PHYS_PAGE_SIZE);
	
	zone->first_free_addr = new_first_free;
	return naddr;		
    }

    panic("pmm: AllocatePhysical: phys memory exhausted, no suitable zones");
    return 0;
    
}

/**
 * Check if 'count' pages are free in the bitmap 'alloc_bitmap_bitmap',
 * starting from 'page_offset'
 *
 * @return True if we have sufficient free pages, false if not
 */
bool PMM::CheckIfPagesFree(char const* alloc_bitmap,
			   unsigned int page_offset,
			   size_t page_count)
{
    // 8 bits in a byte.
    unsigned byte_offset = page_offset / 8;
    unsigned bit_offset = page_offset % 8;

    /* First check, then map */
    for (unsigned bit_idx = 0; bit_idx < page_count; bit_idx++) {

	// Check if mapped
	if (alloc_bitmap[byte_offset] & (1 << bit_offset)) {
	    Log::Write(Error, "pmm", "page already mapped in bitmap offset %d + "
		       "(%d * 8) + %d pages)",
		   page_offset, byte_offset, bit_offset);
	    return false;
	}
	
	bit_offset++;

	if (bit_offset >= 8) {
	    bit_offset = 0;
	    byte_offset++;
	}
    }

    return true;
}

/**
 * Map pages in bitmap 'alloc_zones_bitmap', starting in
 * page_offset until page_count
 */
void PMM::MapPagesInBitmap(char* alloc_zones_bitmap,
			   unsigned int page_offset,
			   size_t page_count)
{
    unsigned byte_offset = page_offset / 8;
    unsigned bit_offset = page_offset % 8;

    for (unsigned bit_idx = 0; bit_idx < page_count; bit_idx++) {
	// If not, map
	alloc_zones_bitmap[byte_offset] |= (1 << bit_offset);
	
	bit_offset++;

	if (bit_offset >= 8) {
	    bit_offset = 0;
	    byte_offset++;
	}
	
    }

}

/**
 * Check if you can allocate 'n' pages starting from the first free address
 * 
 * @returns -1 if you can't because no memory, 0 if because it's fragmented
 * e.g, because there's someone in there, or 1 if you can
 */
int PMM::CheckPages(size_t n)
{
    return 0;
}

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
phys_t PMM::MapPages(phys_t addr, size_t n)
{
    PMMZone* zone = this->FindZone(addr);
    if (!zone) {
	Log::Write(Error, "pmm",
		   "MapPages: couldn't find a suitable zone for 0x%08x", addr);
	return 0xffffffff;
    }

    if (addr < zone->first_free_addr) {
	Log::Write(Error, "pmm",
		   "MapPages: can't map %u pages from address 0x%08x: less that the first free address", n, addr);
	return 0xffffffff;
    }
    
    const unsigned addr_offset = (addr - zone->start);
    const unsigned page_offset = addr_offset / PHYS_PAGE_SIZE;

    if (!this->CheckIfPagesFree((char const*)zone->alloc_bitmap,
				page_offset, n)) {
	Log::Write(Error, "pmm",
		   "error: page at %08x already mapped", addr_offset);
	return 0xffffffff;	
    }

    this->MapPagesInBitmap(zone->alloc_bitmap, page_offset, n);
 
    if (addr == zone->first_free_addr) {
	// Update the first free address
	auto offset = n*PHYS_PAGE_SIZE;
	zone->first_free_addr += offset;
    }

    return addr;
}

/**
 * Unmap 'n' pages starting from 'addr'
 * Can be used with AllocatePhysical or MapPages, because both map pages
 *
 * Return the number of pages unmapped
 */
size_t PMM::UnmapPages(phys_t addr, size_t n)
{
    PMMZone* zone = this->FindZone(addr);
    if (!zone) {
	Log::Write(Error, "pmm",
		   "UnmapPages: couldn't find a suitable zone for 0x%08x", addr);
	return 0xffffffff;
    }

    if (addr < zone->first_free_addr) {
	// Pad first free address to the nearest page
	zone->first_free_addr = (addr & ~0xfff);
    }

    const unsigned addr_offset = (addr - zone->start);
    const unsigned page_offset = addr_offset / PHYS_PAGE_SIZE;
    unsigned byte_offset = page_offset / 8;
    unsigned bit_offset = page_offset % 8;

    for (unsigned bit_idx = 0; bit_idx < n; bit_idx++) {
	
	zone->alloc_bitmap[byte_offset] ^= (1 << bit_offset);
	
	bit_offset++;

	if (bit_offset >= 8) {
	    bit_offset = 0;
	    byte_offset++;
	}
    }

    return n;
}


/* Find the zone that the address 'addr' is from */
PMMZone* PMM::FindZone(phys_t addr)
{
    PMMZone* best_zone = NULL;
    for (unsigned i = 0; i < _mmap_count; i++) {
	PMMZone* zone = &_mmap[i];
	uintptr_t zone_end = zone->start + (PHYS_PAGE_SIZE * zone->pagecount) - 1;
	Log::Write(Debug, "pmm", "find zone for %x (%d, %x -> %x)",
		   addr, i, zone->start, zone_end);
	
	if (addr >= zone->start && addr <= zone_end) {
	    best_zone = zone;
	    break;
	}	    	
    }
    
    return best_zone;
}

