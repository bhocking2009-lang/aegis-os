#ifndef AEGIS_EFI_STUB_H
#define AEGIS_EFI_STUB_H
/* Aegis B01: freestanding x86_64 UEFI interfaces. No firmware writes. */
#include <stdint.h>
#include <stddef.h>
#define EFIAPI __attribute__((ms_abi))
typedef uint64_t UINTN;
typedef uint64_t EFI_STATUS;
typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef uint16_t CHAR16;
typedef void *EFI_HANDLE;
typedef void *EFI_EVENT;
typedef struct { uint32_t Data1; uint16_t Data2, Data3; uint8_t Data4[8]; } EFI_GUID;
typedef struct {
  uint64_t Signature;
  uint32_t Revision, HeaderSize, CRC32, Reserved;
} EFI_TABLE_HEADER;
#define EFI_SUCCESS ((EFI_STATUS)0)
#define EFIERR(n) ((EFI_STATUS)(0x8000000000000000ULL | (uint64_t)(n)))
#define EFI_ERROR(s) (((s) & 0x8000000000000000ULL) != 0)
#define EFI_INVALID_PARAMETER EFIERR(2)
#define EFI_UNSUPPORTED EFIERR(3)
#define EFI_BUFFER_TOO_SMALL EFIERR(5)
#define EFI_NOT_READY EFIERR(6)
#define EFI_DEVICE_ERROR EFIERR(7)
#define EFI_OUT_OF_RESOURCES EFIERR(9)
#define EFI_NOT_FOUND EFIERR(14)
enum EFI_MEMORY_TYPE {
  EfiReservedMemoryType=0, EfiLoaderCode=1, EfiLoaderData=2,
  EfiBootServicesCode=3, EfiBootServicesData=4,
  EfiRuntimeServicesCode=5, EfiRuntimeServicesData=6,
  EfiConventionalMemory=7, EfiUnusableMemory=8,
  EfiACPIReclaimMemory=9, EfiACPIMemoryNVS=10,
  EfiMemoryMappedIO=11, EfiMemoryMappedIOPortSpace=12,
  EfiPalCode=13, EfiPersistentMemory=14, EfiUnacceptedMemoryType=15
};
typedef struct {
  uint32_t Type;
  uint32_t Pad;
  EFI_PHYSICAL_ADDRESS PhysicalStart;
  uint64_t VirtualStart;
  uint64_t NumberOfPages;
  uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;
typedef struct { uint16_t ScanCode, UnicodeChar; } EFI_INPUT_KEY;
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
  EFI_STATUS (EFIAPI *Reset)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *, uint8_t);
  EFI_STATUS (EFIAPI *ReadKeyStroke)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *, EFI_INPUT_KEY *);
  EFI_EVENT WaitForKey;
};
struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  EFI_STATUS (EFIAPI *Reset)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *, uint8_t);
  EFI_STATUS (EFIAPI *OutputString)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *, const CHAR16 *);
  void *TestString, *QueryMode, *SetMode, *SetAttribute, *ClearScreen;
  void *SetCursorPosition, *EnableCursor, *Mode;
};
typedef struct _EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
struct _EFI_BOOT_SERVICES {
  EFI_TABLE_HEADER Hdr;
  void *RaiseTPL, *RestoreTPL;
  void *AllocatePages, *FreePages;
  EFI_STATUS (EFIAPI *GetMemoryMap)(UINTN *, EFI_MEMORY_DESCRIPTOR *, UINTN *, UINTN *, uint32_t *);
  EFI_STATUS (EFIAPI *AllocatePool)(uint32_t, UINTN, void **);
  EFI_STATUS (EFIAPI *FreePool)(void *);
  void *CreateEvent, *SetTimer;
  EFI_STATUS (EFIAPI *WaitForEvent)(UINTN, EFI_EVENT *, UINTN *);
  void *SignalEvent, *CloseEvent, *CheckEvent;
  void *InstallProtocolInterface, *ReinstallProtocolInterface, *UninstallProtocolInterface;
  EFI_STATUS (EFIAPI *HandleProtocol)(EFI_HANDLE, const EFI_GUID *, void **);
  void *Reserved, *RegisterProtocolNotify, *LocateHandle, *LocateDevicePath;
  void *InstallConfigurationTable, *LoadImage, *StartImage, *Exit, *UnloadImage;
  void *ExitBootServices, *GetNextMonotonicCount, *Stall, *SetWatchdogTimer;
  void *ConnectController, *DisconnectController, *OpenProtocol, *CloseProtocol;
  void *OpenProtocolInformation, *ProtocolsPerHandle;
  EFI_STATUS (EFIAPI *LocateHandleBuffer)(uint32_t, const EFI_GUID *, void *, UINTN *, EFI_HANDLE **);
  EFI_STATUS (EFIAPI *LocateProtocol)(const EFI_GUID *, void *, void **);
  void *InstallMultipleProtocolInterfaces, *UninstallMultipleProtocolInterfaces;
  void *CalculateCrc32, *CopyMem, *SetMem, *CreateEventEx;
};
typedef struct {
  EFI_TABLE_HEADER Hdr;
  CHAR16 *FirmwareVendor;
  uint32_t FirmwareRevision;
  EFI_HANDLE ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  EFI_HANDLE ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
  EFI_HANDLE StandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
  void *RuntimeServices;
  EFI_BOOT_SERVICES *BootServices;
  UINTN NumberOfTableEntries;
  void *ConfigurationTable;
} EFI_SYSTEM_TABLE;
typedef struct _EFI_PCI_IO_PROTOCOL EFI_PCI_IO_PROTOCOL;
/* PCI config access has no BarIndex; MMIO and port I/O access do. */
typedef EFI_STATUS (EFIAPI *EFI_PCI_IO_PCI_ACCESS)(
  EFI_PCI_IO_PROTOCOL *, uint32_t, uint32_t, UINTN, void *);
typedef EFI_STATUS (EFIAPI *EFI_PCI_IO_MEM_IO_ACCESS)(
  EFI_PCI_IO_PROTOCOL *, uint32_t, uint8_t, uint64_t, UINTN, void *);
typedef struct {
  EFI_PCI_IO_PCI_ACCESS Read;
  EFI_PCI_IO_PCI_ACCESS Write;
} EFI_PCI_IO_PROTOCOL_PCI_ACCESS;
typedef struct {
  EFI_PCI_IO_MEM_IO_ACCESS Read;
  EFI_PCI_IO_MEM_IO_ACCESS Write;
} EFI_PCI_IO_PROTOCOL_MEM_IO_ACCESS;
struct _EFI_PCI_IO_PROTOCOL {
  void *PollMem, *PollIo;
  EFI_PCI_IO_PROTOCOL_MEM_IO_ACCESS Mem, Io;
  EFI_PCI_IO_PROTOCOL_PCI_ACCESS Pci;
  void *CopyMem, *Map, *Unmap, *AllocateBuffer, *FreeBuffer, *Flush;
  EFI_STATUS (EFIAPI *GetLocation)(EFI_PCI_IO_PROTOCOL *, UINTN *, UINTN *, UINTN *, UINTN *);
  void *Attributes, *GetBarAttributes, *SetBarAttributes;
  uint64_t RomSize;
  void *RomImage;
};
static const EFI_GUID AEGIS_PCI_IO_GUID = {
  0x4cf5b200U, 0x68b8U, 0x4ca5U, {0x9e,0xec,0xb2,0x3e,0x3f,0x50,0x02,0x9a}
};
#define EFI_BY_PROTOCOL 2U
#define EFI_PCI_WIDTH_UINT32 2U
#endif
