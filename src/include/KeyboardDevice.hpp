#pragma once

/*
  Represents a keyboard device to the system

  Copyright (C) 2018 Arthur M
*/

#include <stdint.h>
#include <Device.hpp>

namespace annos {

    /*
     * Enum code representing keycodes for the systems
     * They aren't the same as the ones from the hardware, and the driver
     * will have to convert.
     *
     */
    enum KeyCode {
	KeyNull = 0,

	// Letter keys
	KeyA = 1, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, KeyK,
	KeyL, KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT, KeyU, KeyV,
	KeyW, KeyX, KeyY, KeyZ,

	// Numeric keys
	Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9, Key0,

	// Some other nice-to-have chars that I don't know how to group
	KeySlash, KeyMinus, KeyPlus,

	// Enter, space, super (aka WinKey®) and dead keys
	KeyEnter, KeySpace, KeySuper,
	
	// TODO: Arrows, home, end, page down and up, numpad keys...

	// The 'locks'
	KeyCapsLock = 0x80, KeyNumLock, KeyScrollLock, KeyCtrl, KeyAlt, KeyShift,
	
	KeyMaxKeys
    };

    /*
     * Key information received from the device to this class
     */
    struct KeyDevice {
	KeyCode keycode;
	bool pressed; // true if pressed, false if released

	// The hardware keycode, for 'smart' software that might want to
	// interpret them directly. TODO: Keep this or not?
	uint32_t hw_keycode; 
    };

    /*
     * Key information returned from this class
     */
    struct Key {
	KeyCode keycode;
	bool pressed;
	
	struct {
	    unsigned isctrl:1;
	    unsigned isalt:1;
	    unsigned isshift:1;
	    unsigned rsvd:5;
	} deadkeys;
	struct {
	    unsigned iscaps:1;
	    unsigned isnum:1;
	    unsigned isscroll:1;
	    unsigned rsvd:5;
	} locks;
	
	// String representation of the key, or \0 for none
	char str[4];
    };
    

    /*
     * Class for the keyboard device.
     *
     *  The keyboard device that subclass this only gets keypresses and 
     * returns them to the user.
     *  It doesn't try to convert the key to some ASCII value, this is 
     * responsability of the upper class.
     */
    class KeyboardDevice : public Device {
    private:
	bool isnum=false, iscaps=false, isscroll=false;

	// Key map for all the pressed chars
	bool pressed_keys[KeyCode::KeyMaxKeys];

	
	
    protected:
	/**
	 * Gets a key from the device
	 * 
	 * @return true if there's a key pressed in the buffer, false if
	 * not. If true, fills key information in the 'KeyDevice' struct.
	 */
	virtual bool GetDeviceKey(KeyDevice& d) = 0;

	/**
	 * Turns the LED of the 'lock' keys on or off, depending on the state
	 * parameter
	 */
	virtual void TurnLED(KeyCode kc, bool state) = 0;
	
    public:
	KeyboardDevice(const char* tag, const char* name)
	    : Device(tag, name)
	    {}
	
	virtual void Initialize() = 0;
	virtual void Reset() = 0;

	/* Same thing as the GetDeviceKey(), only the type changes */
	bool GetKey(Key& key);
	
    };
}
