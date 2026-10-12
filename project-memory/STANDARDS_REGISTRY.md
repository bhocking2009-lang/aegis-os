# Aegis Standards Registry — revision 1
Status: CANONICAL REFERENCE. Standards facts are distinct from architecture choices and implementation claims.
Scope: B01 x86-64 UEFI read-only discovery. Later standards are indexed but not implementation commitments.

## Normative sources
- UEFI Specification 2.10/2.11, UEFI Forum: https://uefi.org/specifications — system table, boot services, PCI I/O, memory map, input/output and GOP.
- Microsoft x64 calling convention: https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention
- Microsoft PE/COFF format: https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
- PCI Express Base Specification (PCI-SIG): https://pcisig.com/specifications
- ACPI specification, UEFI Forum: https://uefi.org/specifications
- EDK II reference implementation: https://github.com/tianocore/edk2 (cross-check, not a substitute for specifications)

## B01 fixed contracts
- x86-64 UEFI firmware interfaces use Microsoft x64 ABI; GCC cross-ABI entry points/function pointers must be annotated ms_abi. Compiler must preserve 16-byte stack alignment and 32-byte caller shadow space.
- EFI_TABLE_HEADER is 24 bytes (Signature 8, Revision 4, HeaderSize 4, CRC32 4, Reserved 4).
- EFI_SYSTEM_TABLE offsets x64: ConIn 0x30, ConOut 0x40, BootServices 0x60. Assert rather than assume.
- EFI_BOOT_SERVICES x64 offsets: RaiseTPL 0x18; RestoreTPL 0x20; AllocatePages 0x28; FreePages 0x30; GetMemoryMap 0x38; AllocatePool 0x40; FreePool 0x48. All additional offsets must be independently verified against the normative layout before use.
- EFI_STATUS is pointer-sized unsigned; EFIERR(n) constructs error status by setting bit 63; EFI_ERROR(status) is a predicate testing bit 63. EFI_BUFFER_TOO_SMALL = EFIERR(5). EFI_NOT_FOUND = EFIERR(14).
- GetMemoryMap takes five arguments (MemoryMapSize, MemoryMap, MapKey, DescriptorSize, DescriptorVersion); initial size-zero call normally returns EFI_BUFFER_TOO_SMALL. AllocatePool is (PoolType, Size, Buffer**), FreePool is (Buffer).
- EFI_MEMORY_DESCRIPTOR x64 has Type uint32, padding uint32, PhysicalStart uint64, VirtualStart uint64, NumberOfPages uint64, Attribute uint64; sizeof 40. Iterate by firmware-provided DescriptorSize, not sizeof.
- UEFI console strings are CHAR16 (16-bit); EFI_INPUT_KEY has uint16 ScanCode and uint16 UnicodeChar.
- PCI device discovery through UEFI PCI_IO_PROTOCOL or PCI enumeration protocol, with spec-correct GUIDs, structures, and PCI configuration reads; do not claim RX 6800 XT detected until measured on hardware.
- UEFI PCI I/O protocol access signatures differ: `Pci.Read/Write(This, Width, Offset, Count, Buffer)` (5 args), whereas `Mem.Read/Write` and `Io.Read/Write` require `(This, Width, BarIndex, Offset, Count, Buffer)` (6 args; BarIndex is UINT8). Keep the structures' 16-byte Read/Write pair layout and assert both offsets and signatures.
- PE32+ AMD64 EFI application has subsystem 10; verify headers and relocations in actual binary.
- Removable x64 boot fallback path: \\EFI\\BOOT\\BOOTX64.EFI on supported FAT EFI media.
- B01 is read-only. No target disk writes, firmware variable changes, NVRAM entry changes, or permanent boot order changes.

## Subsequent contracts (NOT B01 scope)
- ACPI: RSDP/XSDT and table checksums.
- Graphics: UEFI Graphics Output Protocol and framebuffer pixel formats.
- Storage: UEFI Block I/O protocol and media identification.
- Firmware testing: QEMU with EDK II OVMF; capture actual boot output and tool exit evidence.

## Change control
Agent may propose a correction with exact normative reference and test; coordinator verifies and records it in DECISIONS.md. No silent rewrite of canonical contracts. Source code must include compile-time layout assertions. Passing compilation does not prove runtime correctness.

