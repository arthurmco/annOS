#include <arch/x86/VMM.hpp>
#include <Log.hpp>
#include <libk/panic.h>


using namespace annos;
using namespace annos::x86;

/** 
 * Represents a kernel virtual memory zone 
 *
 * It is mapped to the VMMZone enum by the index.
 * This means that, for example, vzones[ZUser] will have information 
 *  about the ZUser zone.
 */
struct VMMZoneStruct {
    virt_t addr_start; // The starting virtual address of that zone
    virt_t addr_end;   // The ending virtual address
    virt_t last_vaddr; // The last vaddr allocated. Invalidate this on cr3 switches.
};

const VMMZoneStruct vzones[MaxZones] = {
    {.addr_start = 0x0, .addr_end = 0xBFFFFFFF}, // User zone
    {.addr_start = 0xC0000000, .addr_end = 0xffffffff} // Kernel zone
};

/**
 * Structure of a page directory
 */
union PageDir {
    // Physical address of that entry
    // At it we have 1024 page tables
    phys_t addr; 
    struct {
        unsigned present:1;         // It's present in memory
        unsigned writable:1;        // It's writable
        unsigned user:1;            // It's accessable in user-mode ( Spectre says otherwise :P )

	/* If true, write-throught (write in cache and memory). 
	 * If false, write-back (write in cache and later in memory).
	 */
        unsigned write_through:1;

	// If true, page won't be cached. If false, it will be
        unsigned no_cache:1;

	// True if the processor accessed the page directory.
        unsigned accessed:1;
	
        unsigned zero:1;

	// True if we have 4MB pages. (No support currently)
        unsigned page_size:1;

	// Unused here
        unsigned global:1;

	// Bits available for something I need to think of. Probably disk-swapping related.
        unsigned avail:3;

	// Dir entry physical address >> 12
        unsigned dir_location:20;
    };

} __attribute__((packed));

/*
 * Structure of a page table
 */
union PageTable {
    /* Page physical address */
    uint32_t addr;
    struct {
        unsigned present:1;        // Present?
        unsigned writable:1;       // Writable?
        unsigned user:1;           // Accessible in user mode?
        unsigned write_through:1;  // Write-through cache?
        unsigned no_cache:1;       // Is cache disabled?
        unsigned accessed:1;       // The processor accessed this page?
        unsigned dirty:1;          // The processor wrote to this page?
        unsigned zero:1;

	// If true, then this page won't be flushed from TLB when you set cr3
	// (Not supported yet)	
        unsigned global:1;         
	unsigned avail:3;
	unsigned addr_location:20;
	
    };
} __attribute__((packed));


/**
 * Pointer to the PMM
 * The virtual memory manager will encapsulate it.
 * You shouldn't access it directly anyway, or you wouldn't get a 
 * virtual address for anything
 */
annos::PMM* VMM::_pmm;

/**
 * The physical address of the page directory base register, 
 * aka cr3
 */
phys_t VMM::kernel_cr3_base;

constexpr virt_t kernel_virt_cr3_base = 0xfffff000;

/**
 * Check if it can map 'n' pages of physical address 'phys' to virtual
 * 'virt'.
 * Set allow_nc to false to fail if it couldn't allocate contiguous
 * physical pages.
 *
 * @return false if it won't be able to map, true if it will
 */
bool VMM::CheckPhysicalToVirtual(phys_t phys, size_t n,
					virt_t virt,
					bool allow_nc)
{

      
}
	
/**
 * Map 'n' pages of physical address 'phys' to the virtual address 'virt'
 * Set allow_nc to false to fail if it couldn't allocate contiguous
 * physical pages.
 *
 * @return the number of contiguous pages mapped (e.g, if you mapped 6
 * virtual pages to 2 contiguous phys pages, and the 4 contiguous pages,
 * this function will return 2. Or return -1 if it couldn't map.
 */
int VMM::MapPhysicalToVirtual(phys_t phys, size_t n,
					 virt_t virt,
					 bool allow_nc)
{

    unsigned dirindex, tableindex;
    tableindex = (virt >> 12) & 0x3ff;
    dirindex = (virt >> 22);

    PageDir* pdir = (PageDir*)kernel_virt_cr3_base;
    if (!pdir[dirindex].present) {
	Log::Write(Fatal, "vmm", "page dir %d isn't mapped", dirindex);
	panic("vmm: page directory isn't mapped");
    }

    
    
    
}

void VMM::Init(annos::PMM* pmm, const uintptr_t phys_cr3_base,
		      virt_t kernel_start, virt_t kernel_end)
{
    Log::Write(Debug, "vmm", "phys_cr3 %08x, virtual_kstart %08x, virtual_kend %08x", phys_cr3_base, kernel_start, kernel_end);
    
    /* 2 things:
     *   1: Map the last page directory entry to the page directory
     *      This ensures that we'll be able to access it at address
     *      0xFFFFF000
     *   2: Unmap the first memory region, the identity mapped one
     */
    PageDir* pdir = (PageDir*)phys_cr3_base;
    

    Log::Write(Debug, "vmm", "pdir[1023].addr - %08x",
	       pdir[1023].addr);

    pdir[1023].addr = phys_cr3_base | 0x3; // Map last dir to itself, present and writeable.

    Log::Write(Debug, "vmm", "pdir[1023].addr - %08x",
	       pdir[1023].addr);

    
    PageDir* identity_pdir = (PageDir*)kernel_virt_cr3_base;
    Log::Write(Debug, "vmm", "identity_pdir[0] = %08x",
	       identity_pdir[0]);

    PageTable* identity_ptbl = (PageTable*)(identity_pdir[0].addr & ~0x3ff);
    Log::Write(Debug, "vmm", "identity_ptbl[0] = %08x",
	       identity_ptbl[0]);

    identity_ptbl[0].addr = 0;

    unsigned* ip = (unsigned*)NULL;
    *ip = 0x220;

    // Reload cr3, this flushes the TLB.
    // (Next framebuffer access might cause a page fault)
    asm("mov %0, %%cr3" : : "r"(phys_cr3_base & ~0x3ff));
}

/**
 * Allocate next avaliable 'n' pages from zone 'zone'.
 * Return the allocated virtual address from that zone
 */
virt_t VMM::AllocateVirtual(size_t n, VMMZone zone)
{

}

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
virt_t VMM::AllocateVirtualPhysical(phys_t* rphys, PMMZone pzone,
					   size_t n, VMMZone vzone)
{

}

/**
 * Allocate the next avaliable 'n' virtual pages and map them to the
 * specified physical address 'phys'
 *
 * This function might be useful for MMIO, that require a specific
 * physical address
 *
 * Return the mapped virtual address for that physical address
 */
virt_t VMM::MapPhysicalAddress(phys_t phys, size_t n,
				      VMMZone vzone)
{

}

	

