#pragma once

/**
 * Virtual memory allocator for the x86
 *
 * Copyright (C) 2018 Arthur M
 */

#include <stdint.h>
#include <stddef.h>
#include <PMM.hpp>

typedef uintptr_t virt_t;


namespace annos::x86 {

    enum VMMZone {
	ZUser,
	ZKernel,
	ZAppLoader,
	MaxZones
    };

    #define VMM_PAGE_SIZE 4096

    class VMM {
    private:
	static annos::PMM* _pmm;
	static phys_t kernel_cr3_base;

	/**
	 * Map a directory entry index 'dirindex' in the current page
	 * directory
	 *
	 * Return its physical address
	 */
	static phys_t MapPageDirectoryIndex(unsigned dirindex);

	/**
	 * Check if it can map 'n' pages of physical address 'phys' to virtual
	 * 'virt'.
	 * Set allow_nc to false to fail if it couldn't allocate contiguous
	 * physical pages.
	 *
	 * @return false if it won't be able to map, true if it will
	 */
	static bool CheckPhysicalToVirtual(phys_t phys, size_t n, virt_t virt);
	
	/**
	 * Map 'n' pages of physical address 'phys' to the virtual address 'virt'
	 * Set allow_nc to false to fail if it couldn't allocate contiguous
	 * physical pages.
	 *
	 * @return the number of contiguous pages mapped (e.g, if you mapped 6
	 * virtual pages to 2 contiguous phys pages, and the 4 contiguous pages,
	 * this function will return 2. Or return -1 if it couldn't map.
	 */
	static int MapPhysicalToVirtual(phys_t phys, size_t n, virt_t virt);

	/**
	 * Unmap 'n' bits starting from physical address 'virt'
	 */
	static int UnmapVirtual(virt_t virt, size_t n);
	
    public:
	static void Init(annos::PMM* pmm, uintptr_t phys_cr3_base,
			 virt_t kernel_start, virt_t kernel_end);

	/**
	 * Allocate next avaliable 'n' pages from zone 'zone'.
	 * Return the allocated virtual address from that zone
	 */
	static virt_t AllocateVirtual(size_t n = 1,
				      VMMZone zone = VMMZone::ZKernel);

	/**
	 * Allocate next avaliable 'n' virtual pages from zone 'zone', but 
	 * also return the used physical address mapped to that virtual 
	 * address on 'rphys'.
	 * 'pzone' is the required physical memory zone of the wished address
	 *
	 * This function might be useful for drivers allocating DMA buffers
	 * They require the physical address, but, besides the zone, it can
	 * be any phys address
	 *
	 * Return the allocated virtual address
	 */
	static virt_t AllocateVirtualPhysical(phys_t* rphys, PMMZoneType pzone,
					      size_t n = 1,
					      VMMZone vzone = VMMZone::ZKernel);

	/**
	 * Allocate the next avaliable 'n' virtual pages and map them to the
	 * specified physical address 'phys'
	 *
	 * This function might be useful for MMIO, that require a specific
	 * physical address
	 *
	 * Return the mapped virtual address for that physical address
	 */
	static virt_t MapPhysicalAddress(phys_t phys,
					 size_t n = 1,
					 VMMZone vzone = VMMZone::ZKernel);

    };
    
}
