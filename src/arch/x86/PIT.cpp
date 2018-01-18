#include <arch/x86/PIT.hpp>
#include <arch/x86/IO.hpp>
#include <arch/x86/IRQHandler.hpp>

#include <libk/panic.h>
#include <Log.hpp>

using namespace annos::x86;

// The normal clock of the PIT, in HZ

// Changes the new timer clock to 'hz'
void PIT::SetTimer(uint16_t hz)
{
    constexpr unsigned pit_rate = 1193182;
    uint16_t divisor = pit_rate / hz;
    
    out8(0x43, 0x36);
    out8(0x40, (divisor & 0xff));
    out8(0x40, (divisor >> 8));

    Log::Write(LogLevel::Info, "pit", "frequency set to %d Hz", hz);
}

/** 
 *  Device initialization
 *  Sets the clock and the IRQ handler
 */
void PIT::Initialize()
{
    this->SetTimer(1000);
    IRQHandler::SetHandler(0, [](IRQRegs* r) {
	    static unsigned i = 0;
	    i++;

	    if (i % 1000 == 0 && i > 0)
		Log::Write(LogLevel::Debug, "pit", "1 second has passed");
	});
}

/**
 * Device reset.
 * For now, just panic. We might not need to reset this.
 */
void PIT::Reset()
{
    panic("pit doesn't support reset now");
}

