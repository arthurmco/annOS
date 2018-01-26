#include <PCIBus.hpp>
#include <stdint.h>
#include <Log.hpp>
#include <arch/x86/IO.hpp>
#include <libk/stdlib.h>
#include <libk/panic.h>

using namespace annos;
using namespace annos::x86;

static PCIAddressQuery MakePCIAddrQuery(unsigned bus, unsigned dev, unsigned fun,
					unsigned reg)
{
    PCIAddressQuery p;
    p.enable = 1;
    p.rsvd = 0;
    p.bus = bus;
    p.device = dev;
    p.function = fun;
    p.reg = reg;
    p.align = 0;
    return p;			 
}

/**
 * Make a read with 'size' bytes in the PCI register 'idx' of device
 * 'dev'
 *
 * @return the content read
 *
 * @remarks Note that 'size' can only be a multiple of 8
 */
template<uint8_t size>
unsigned PCIBus::ReadPCIRegister(PCIDev* dev, unsigned idx)
{
    assert(size % 8 == 0);
    assert(size <= 32);

    // ridx is that will be used. PCI supports directly querying only
    //registers that are multiples of 4
    unsigned ridx = (idx & ~0x3);
    unsigned roff = (idx & 0x3);
    
    auto qry = MakePCIAddrQuery(dev->bus, dev->dev, dev->func, ridx>>2);
    ::x86::out32(CONFIG_ADDRESS, qry.data);

    unsigned data = ::x86::in32(CONFIG_DATA);

    unsigned mask = 0xffffffff;
    if (size < 32)
	mask = (1 << size) - 1;

    unsigned ret = (data >> (roff * 8));
    ret &= mask;
    return ret;
}

/** 
 *  Initializate and discover the devices connected there
 */
void PCIBus::Initialize()
{
    Log::Write(Info, "pcibus", "Querying PCI devices");

    constexpr int maxpcidevs = 16;
    unsigned pidx = 0;
    PCIDev pcidevs[maxpcidevs];

    /* Query devices and list them */
    for (unsigned bus = 0; bus < 255; bus++) {
	for (unsigned dev = 0; dev < 32; dev++) {
	    for (unsigned fun = 0; fun < 8; fun++) {

		// Mount the query and sent to the pci bus */
		const unsigned pcibsize = sizeof(PCIDev) / sizeof(uint32_t);
		uint32_t pcibytes[pcibsize];

		PCIAddressQuery query = MakePCIAddrQuery(bus, dev, fun, 0);
		bool exists = false;
		
		for (unsigned queryidx = 0; queryidx < pcibsize; queryidx++) {
		    query.reg = queryidx;
		    ::x86::out32(CONFIG_ADDRESS, query.data);
		    pcibytes[queryidx] = ::x86::in32(CONFIG_DATA);
		    
		    // If the device is non-existant, break
		    if (pcibytes[0] == 0xffffffff) {
			break;
		    } else {
			exists = true;
		    }
		}

		if (!exists) {
		    continue;
		} else {
		    Log::Write(Debug, "pcibus", "found device at %02x:%02x:%x",
			       bus, dev, fun);
		    memcpy(&pcidevs[pidx].reginfo, &pcibytes[0],
			   sizeof(PCIRegister));

		    pcidevs[pidx].bus = bus;
		    pcidevs[pidx].dev = dev;
		    pcidevs[pidx].func = fun;

		    pidx++;
		    /* If bit 7 of type is set, then the device has multiple
		       functions. If not, then it's safe to break 
		    */
		    if (fun == 0 && pcidevs[pidx-1].reginfo.header_type < 0x80)
			break;
		    
		}
	    }
	}
    }

    Log::Write(Info, "pcibus", "%d PCI devices discovered", pidx);

    for (unsigned i = 0; i < pidx; i++) {
	PCIRegister* pr = &pcidevs[i].reginfo;
	Log::Write(Info, "pcibus", "%d:%d.%d -> \033[36m%04x:%04x\033[0m, command %04x, status %04x, type %02x",
		   pcidevs[i].bus, pcidevs[i].dev, pcidevs[i].func,
		   pr->vendor, pr->device, pr->command, pr->status, pr->header_type);
	Log::Write(Info, "pcibus", "         class %02x:%02x, rev %02x progid %02x", pr->classcode, pr->subclass, pr->rev, pr->prog_id);

	switch ((pr->header_type & 0xf)) {
	case 0:
	    for (unsigned int baridx = 0; baridx < 6; baridx++) {
		if (pr->dev.bar[baridx]) {

		    unsigned bar = pr->dev.bar[baridx];
		    uintptr_t addr = 0;
		    const char* type;
		    if (bar & 0x1) {
			type = "I/O port";
			addr = bar & ~0x3;
		    } else {
			type = "memory";
			addr = bar & ~0xf;
		    }
		    
		    
		    Log::Write(Info, "pcibus", "         bar[%d] = (%s at %x)",
			       baridx, type, addr);
		}
	    }
	    break;
        case 1:
	    for (unsigned int baridx = 0; baridx < 2; baridx++) {
		if (pr->pci2pci.bar[baridx]) {
		    unsigned bar = pr->pci2pci.bar[baridx];
		    uintptr_t addr = 0;
		    const char* type;
		    if (bar & 0x1) {
			type = "I/O port";
			addr = bar & ~0x3;
		    } else {
			type = "memory";
			addr = bar & ~0xf;
		    }
		    
		    
		    Log::Write(Info, "pcibus", "         bar[%d] = (%s at %x)",
			       baridx, type, addr);
		}

	    }
	    break;
	    
	}
	
	if (pr->dev.interrupt_line > 0)
	    Log::Write(Info, "pcibus", "         interrupt %d at pin %02x", pr->dev.interrupt_line, pr->dev.interrupt_pin);
    }


}


/**
 * Do nothing
 * TODO: _Re_discover the devices?
 */
void PCIBus::Reset(){}
