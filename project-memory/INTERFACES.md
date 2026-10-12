# Aegis Interfaces

UEFI x86-64 Microsoft ABI. EFI GetMemoryMap requires five arguments: UINTN *MemoryMapSize, EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN *MapKey, UINTN *DescriptorSize, UINT32 *DescriptorVersion. EFI_BUFFER_TOO_SMALL is EFI_ERROR(5), high error bit set. EFI_BOOT_SERVICES must have spec-correct layout; no invented offsets. Main build toolchain MSYS2 UCRT64 GCC. Bravo resident endpoint protected.
