#include <arch/x86/SMBIOS.hpp>
#include <libk/stdio.h> // TODO: move strlen it to string.h
#include <libk/stdlib.h>
#include <libk/panic.h>
#include <Log.hpp>

using namespace annos;
using namespace annos::x86;


bool CheckIfChecksumValid(SMBiosEntry* e)
{
    char* smb_bytes = (char*)e;
    unsigned char chksum_total = 0;

    for (unsigned i = 0; i < e->length; i++) {
	chksum_total += smb_bytes[i];
    }
    
    Log::Write(Debug, "smbios", "Checksum is %d, should be %d at 0x%x",
	       chksum_total, 0, e);
    return (chksum_total == 0);
}

bool SMBios::Detect()
{
    /* Check if we find the SMBIOS entry point 
       It's composed by the characters '_SM_', and it lies in a 16-byte
       boundary (aka address ended with 0), between physical addresses 
       0xf0000 to 0xfffff.
    */

    uintptr_t smbios_addr = 0xf0000;
    const char* entry_magic = "_SM_";
    const char* interm_magic = "_DMI_";

    while (smbios_addr < 0x100000) {
	
	if (!memcmp(entry_magic, (void*)smbios_addr, 4)) {
	    if (!memcmp(interm_magic, (void*)(smbios_addr+0x10), 5)) {

		if (!CheckIfChecksumValid((SMBiosEntry*) smbios_addr)) {
		    smbios_addr += 0x10;
		    continue;
		}

		Log::Write(Info, "smbios", "SMBIOS entry point found at 0x%x",
			   smbios_addr);
		
		this->_smbios_entry_addr = smbios_addr;
		return true;
	    }
	}

	smbios_addr += 0x10;
    }

    Log::Write(Warning, "smbios", "No SMBIOS entry point found");
    return false;
}

/* Gets the string index 'idx' after the smbios table 'tbl' */
const char* SMBios::GetSMBiosString(SMBiosStrHeader* hdr, unsigned idx)
{
    uintptr_t start = ((uintptr_t)hdr) + hdr->length;

    unsigned str_idx = 1; // actual string index
    unsigned str_off = 0;

    // Detect if we have a table with no strings;
    // If so, return.
    // It should start right at the end of the current table
    const char* sch = ((const char*)(start + str_off));
    if (sch[0] == '\0' && sch[1] == '\0')
	return NULL;

    // Process strings until we reach the maximum size possible: the table length
    SMBiosEntry* sb = (SMBiosEntry*)this->_smbios_entry_addr;
    while ((hdr->length + str_off) < sb->smbios_struct_len) {
	if (idx == str_idx)
	    return ((const char*)(start + str_off));

	str_off++;
	const char* ch = ((const char*)(start + str_off));
	// We found a zero termination
	// Next string is another string
	if (ch[0] == '\0') {

	    // We found another one
	    // End of string table
	    if (ch[1] == '\0' && ch[2] != '\0')
		break;
		
	    str_idx++;
	    str_off++;
	} else if (ch[0] <= 31 || ch[0] >= 127) {
	    // Did some shit, got in a place where it shouldn't
	    return NULL;
	}
	
    }

    return NULL;
}

void SMBios::ParseBiosInformation(SMBiosStrHeader* hdr)
{
    SMBios_BiosInfo* binfo = (SMBios_BiosInfo*)hdr;

    const char* vendor = this->GetSMBiosString(hdr, binfo->vendor_strptr);
    const char* version = this->GetSMBiosString(hdr, binfo->version_strptr);
    const char* rdate = this->GetSMBiosString(hdr, binfo->releasedate_strptr);
    
    Log::Write(Info, "smbios", "BIOS vendor: %s", vendor);
    Log::Write(Info, "smbios", "BIOS version: %s", version);
    Log::Write(Info, "smbios", "BIOS release date: %s", rdate);
    Log::Write(Info, "smbios", "BIOS starting segment: %04x",
	       binfo->starting_segment);
    Log::Write(Info, "smbios", "BIOS ROM size: %d kB",
	       64 * (binfo->rom_size+1));
    
    
}

/* Parse SMBIOS board information header */
void SMBios::ParseBoardInformation(SMBiosStrHeader* hdr)
{
    SMBios_Baseboard* binfo = (SMBios_Baseboard*)hdr;

    const char* manufacturer = this->GetSMBiosString(hdr, binfo->manufacturer_sp);
    const char* product = this->GetSMBiosString(hdr, binfo->prodname_sp);
    const char* version = this->GetSMBiosString(hdr, binfo->version_sp);
    const char* serialnum = this->GetSMBiosString(hdr, binfo->serialnumber_sp);

    static const char* boardtype_str[] =
	{"", "Unknown", "Other", "Server Blade", "Connectivity Switch",
	 "System Management Module", "Processor Module", "I/O Module",
	 "Memory Module", "Daughterboard", "Motherboard",
	 "Processor/Memory Module", "Processor/IO Module",
	 "Interconnect board"};
	 
    
    Log::Write(Info, "smbios", "Board type: %s (%x)",
	       boardtype_str[binfo->board_type], binfo->board_type);
    Log::Write(Info, "smbios", "Board Manufacturer: %s", manufacturer);
    Log::Write(Info, "smbios", "Board Product: %s", product);
    Log::Write(Info, "smbios", "Board Version: %s", version);
    Log::Write(Info, "smbios", "Board Serial number: %s",
	       (serialnum) ? serialnum : "<null>");
}


/* Parse SMBIOS system information header */
void SMBios::ParseSysInformation(SMBiosStrHeader* hdr)
{
    SMBios_SysInfo* sinfo = (SMBios_SysInfo*)hdr;

    const char* manufacturer = this->GetSMBiosString(hdr, sinfo->manufacturer_sp);
    const char* product = this->GetSMBiosString(hdr, sinfo->prodname_sp);
    const char* version = this->GetSMBiosString(hdr, sinfo->version_sp);
    const char* serialnum = this->GetSMBiosString(hdr, sinfo->serialnumber_sp);
    const char* sku = this->GetSMBiosString(hdr, sinfo->serialnumber_sp);
    const char* model = this->GetSMBiosString(hdr, sinfo->serialnumber_sp);

    Log::Write(Info, "smbios", "Manufacturer: %s",
	       (manufacturer) ? manufacturer : "<null>");
    Log::Write(Info, "smbios", "Product: %s",
	       (product) ? product : "<null>");
    Log::Write(Info, "smbios", "Version: %s",
	       (version) ? version : "<null>");
    Log::Write(Info, "smbios", "Serial number: %s",
	       (serialnum) ? serialnum : "<null>");

    static const char* wake_event_str[] =
	{"Reserved", "Other", "Unknown", "APM Timer", "Modem Ring",
	 "LAN Remote", "Power Switch", "PCI PME#", "AC Power Restored"};
    
    Log::Write(Info, "smbios", "Wake up Type: %s (0x%x)",
	       wake_event_str[sinfo->wakeup_event], sinfo->wakeup_event);

    Log::Write(Info, "smbios", "SKU: %s",
	       (sku) ? sku : "<null>");
    Log::Write(Info, "smbios", "Model: %s",
	       (model) ? model : "<null>");

}

/* Parse SMBIOS processor header */
void SMBios::ParseSysProcessor(SMBiosStrHeader* hdr)
{
    SMBios_Processor* smproc = (SMBios_Processor*)hdr;

    const char* socket = this->GetSMBiosString(hdr, smproc->socket_sp);
    const char* proc_manufacturer = this->GetSMBiosString(
	hdr, smproc->proc_manufacturer_sp);
    const char* proc_version = this->GetSMBiosString(
	hdr, smproc->proc_version_sp);

    static const char* sproc_type[] =
	{"", "Other", "Unknown", "Central Processor", "Math Processor",
	"DSP Processor", "Video Processor"};
				      
    
    Log::Write(Info, "smbios", "Processor Socket: %s", socket);
    Log::Write(Info, "smbios", "Processor Type: %s (%x)",
	       sproc_type[smproc->proc_type], smproc->proc_type);
    Log::Write(Info, "smbios", "Processor Family: %x", smproc->proc_family);
    Log::Write(Info, "smbios", "Processor ID: %08x %08x",
	       ((uint32_t)(smproc->proc_id >> 32)),
	       ((uint32_t)(smproc->proc_id & 0xffffffff)));
    Log::Write(Info, "smbios", "Processor Manufacturer: %s",
	       (proc_manufacturer) ? proc_manufacturer : "<null>");
    Log::Write(Info, "smbios", "Processor Version: %s",
	       (proc_version) ? proc_version : "<null>");
    Log::Write(Info, "smbios", "Bus Clock: %d MHz", smproc->clock_mhz);
    Log::Write(Info, "smbios", "Processor Max Clock: %d MHz",
	       smproc->max_speed_mhz);
    Log::Write(Info, "smbios", "Processor Current Clock: %d MHz",
	       smproc->curr_speed_mhz);
    
    
    
}

	
void SMBios::Initialize()
{
    if (!this->_smbios_entry_addr)
	if (!this->Detect())
	    return;
    
    volatile SMBiosEntry* sm_entry = ( SMBiosEntry* )this->_smbios_entry_addr;

    Log::Write(Info, "smbios", "entry point: len 0x%x, version %d.%d",
	       sm_entry->length, sm_entry->major, sm_entry->minor);

    Log::Write(Info, "smbios", "           maxstructsize %d, ep_revision %02x",
	       sm_entry->max_structure_size, sm_entry->entry_point_revision);

    Log::Write(Info, "smbios", "           smbios_addr %x, len %d, count %d",
	       sm_entry->smbios_struct_addr, sm_entry->smbios_struct_len,
	       sm_entry->smbios_struct_count);


    uintptr_t smbios_ptr = sm_entry->smbios_struct_addr;
    for (int i = 0; i < sm_entry->smbios_struct_count; i++) {
	SMBiosStrHeader* smheader = (SMBiosStrHeader*)smbios_ptr;
	
	Log::Write(Debug, "smbios", "struct %d is type %02d len %d handle %04x "
		   "address 0x%08x",
		   (i+1), smheader->type, smheader->length, smheader->handle,
		   smbios_ptr);

	switch (smheader->type) {
	case 0: this->ParseBiosInformation(smheader); break;
	case 1: this->ParseSysInformation(smheader); break;
	case 2: this->ParseBoardInformation(smheader); break;
	case 4: this->ParseSysProcessor(smheader); break;
	}

	// The string table isn't considered in the smbios length field
	// because i don't know.
	// So we need to traverse it to determine the correct pointer to
	// the next one

	// The formula is:
	// length_of_all_strings + idx + 1
	// idx is for all zeros of the zero-terminated strings
	// +1 is for the last \0

	int max_idx = 1;
	int strlength = 0;
	const char* smstr = this->GetSMBiosString(smheader, max_idx);
	while (smstr != NULL) {
//	    Log::Write(Debug, "smbios", "Found string: %s", smstr);
	    strlength += strlen(smstr)+1;
	    max_idx++;
	    smstr = this->GetSMBiosString(smheader, max_idx);
	}

	if (strlength > 0)
	    strlength += 1;
	else
	    strlength += 2;

	smbios_ptr += (smheader->length + strlength);
    }

}
