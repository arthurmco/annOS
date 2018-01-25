#include <PCIBus.hpp>
#include <stdint.h>
#include <Log.hpp>
#include <arch/x86/IO.hpp>
#include <libk/stdlib.h>

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
		    Log::Write(Debug, "pcibus", "found device #%d at %02x:%02x:%x",
			       pidx, bus, dev, fun);
		    memcpy(&pcidevs[pidx], &pcibytes[0], sizeof(PCIDev));

		    pidx++;
		    /* If bit 7 of type is set, then the device has multiple
		       functions. If not, then it's safe to break 
		    */
		    if (fun == 0 && pcidevs[pidx-1].header_type < 0x80)
			break;
		    
		}
	    }
	}
    }

    Log::Write(Info, "pcibus", "%d PCI devices discovered", pidx);

    for (unsigned i = 0; i < pidx; i++) {
	PCIDev* pd = &pcidevs[i];
	Log::Write(Info, "pcibus", "device %d is %04x:%04x, command %04x, status %04x, type %02x", i, pd->vendor, pd->device, pd->command, pd->status, pd->header_type);
	Log::Write(Info, "pcibus", "            class %02x:%02x, rev %02x progid %02x", pd->classcode, pd->subclass, pd->rev, pd->prog_id);

	switch ((pd->header_type & 0xf)) {
	case 0:
	    for (unsigned int baridx = 0; baridx < 6; baridx++) {
		if (pd->dev.bar[baridx]) {

		    unsigned bar = pd->dev.bar[baridx];
		    uintptr_t addr = 0;
		    const char* type;
		    if (bar & 0x1) {
			type = "I/O port";
			addr = bar & ~0x3;
		    } else {
			type = "memory";
			addr = bar & ~0xf;
		    }
		    
		    
		    Log::Write(Info, "pcibus", "            bar[%d] = (%s at %x)",
			       baridx, type, addr);
		}
	    }
	    break;
        case 1:
	    for (unsigned int baridx = 0; baridx < 2; baridx++) {
		if (pd->pci2pci.bar[baridx]) {
		    unsigned bar = pd->pci2pci.bar[baridx];
		    uintptr_t addr = 0;
		    const char* type;
		    if (bar & 0x1) {
			type = "I/O port";
			addr = bar & ~0x3;
		    } else {
			type = "memory";
			addr = bar & ~0xf;
		    }
		    
		    
		    Log::Write(Info, "pcibus", "            bar[%d] = (%s at %x)",
			       baridx, type, addr);
		}

	    }
	    break;
	    
	}
	
	if (pd->dev.interrupt_line > 0)
	    Log::Write(Info, "pcibus", "            interrupt %d at pin %02x", pd->dev.interrupt_line, pd->dev.interrupt_pin);
    }
}


/**
 * Do nothing
 * TODO: _Re_discover the devices?
 */
void PCIBus::Reset(){}
