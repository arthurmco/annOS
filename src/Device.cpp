#include <Device.hpp>
#include <DeviceIoctl.hpp>

using namespace annos;

int Device::ProcessIoctl(uint32_t function_name,
			 uint64_t v0, uint64_t v1,
			 uint64_t* ret, void* ptr)
{
    // Implement only getting name or tag.

    switch (function_name) {
    case DEVICE_GET_TAG:
	*ret = (uint64_t)this->_tag;
	return 0;
    case DEVICE_GET_NAME:
	*ret = (uint64_t)this->_name;
	return 0;
    default:
	return -1; //return -EINVAL
    }   
}

const char* Device::GetTag() { return _tag; }
const char* Device::GetName() { return _name; }
