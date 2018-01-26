#include <PCIDevice.hpp>
#include <Log.hpp>

using namespace annos;

bool PCIDevice::DetectPCIByClass(uint16_t classcode, uint16_t subclass)
{
    unsigned lsize = 0;
    auto p = _bus->FindPCIByClass(classcode, subclass, lsize);

    if (lsize == 0) return false;

    for (unsigned i = 0; i < lsize; i++) {
	if (p[i].devobj)
	    continue;

	this->pci = &p[i];
	return true;
    }

    Log::Write(Error, "pcidevice", "Found device class %d:%d asked by '%s', but it already had a device object", classcode, subclass, _tag);
    return false;
}
