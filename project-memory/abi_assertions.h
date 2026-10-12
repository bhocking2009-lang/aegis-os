#ifndef AEGIS_B01_ABI_ASSERTIONS_H
#define AEGIS_B01_ABI_ASSERTIONS_H
#include <stddef.h>
/* Include after efi_stub.h. Build must fail on ABI drift. */
_Static_assert(sizeof(EFI_TABLE_HEADER)==24,"UEFI table header size");
_Static_assert(sizeof(EFI_GUID)==16,"UEFI GUID size");
_Static_assert(sizeof(EFI_INPUT_KEY)==4,"UEFI input key size");
_Static_assert(sizeof(EFI_MEMORY_DESCRIPTOR)==40,"UEFI memory descriptor size");
_Static_assert(offsetof(EFI_MEMORY_DESCRIPTOR,NumberOfPages)==0x18,"UEFI descriptor page offset");
_Static_assert(offsetof(EFI_BOOT_SERVICES,GetMemoryMap)==0x38,"GetMemoryMap offset");
_Static_assert(offsetof(EFI_BOOT_SERVICES,AllocatePool)==0x40,"AllocatePool offset");
_Static_assert(offsetof(EFI_BOOT_SERVICES,FreePool)==0x48,"FreePool offset");
_Static_assert(offsetof(EFI_BOOT_SERVICES,HandleProtocol)==0x98,"HandleProtocol offset");
_Static_assert(offsetof(EFI_BOOT_SERVICES,LocateHandleBuffer)==0x138,"LocateHandleBuffer offset");
_Static_assert(offsetof(EFI_BOOT_SERVICES,LocateProtocol)==0x140,"LocateProtocol offset");
_Static_assert(offsetof(EFI_SYSTEM_TABLE,FirmwareVendor)==0x18,"FirmwareVendor offset");
_Static_assert(offsetof(EFI_SYSTEM_TABLE,ConOut)==0x40,"ConOut offset");
_Static_assert(offsetof(EFI_SYSTEM_TABLE,BootServices)==0x60,"BootServices offset");
_Static_assert(offsetof(EFI_PCI_IO_PROTOCOL,Pci)==0x30,"PCI config access offset");
_Static_assert(offsetof(EFI_PCI_IO_PROTOCOL,GetLocation)==0x70,"PCI location offset");
/* Guard function pointer signatures as well as offsets; these are different ABI contracts. */
_Static_assert(__builtin_types_compatible_p(__typeof__(((EFI_PCI_IO_PROTOCOL *)0)->Pci.Read),
  EFI_PCI_IO_PCI_ACCESS),"PCI config read signature");
_Static_assert(__builtin_types_compatible_p(__typeof__(((EFI_PCI_IO_PROTOCOL *)0)->Mem.Read),
  EFI_PCI_IO_MEM_IO_ACCESS),"PCI memory read signature");
_Static_assert(__builtin_types_compatible_p(__typeof__(((EFI_PCI_IO_PROTOCOL *)0)->Io.Read),
  EFI_PCI_IO_MEM_IO_ACCESS),"PCI port I/O read signature");
#endif
