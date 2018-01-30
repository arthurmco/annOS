#include <KeyboardDevice.hpp>

using namespace annos;

/* Same thing as the GetDeviceKey(), only the type changes */
bool KeyboardDevice::GetKey(Key& key)
{
    KeyDevice d;
    bool no_more = false;

    while (!no_more) {
	if (!this->GetDeviceKey(d)) {
	    break;
	}

	uint8_t key = d.keycode;
	pressed_keys[key] = d.pressed;

	
    }

    
}
