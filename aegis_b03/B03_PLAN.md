# Aegis B03 — Controlled GPU MMIO Discovery

Status: DESIGN / SAFETY GATE OPEN. No B03 EFI binary deployed.

## Mission
Demonstrate a bounded, explicitly allowlisted read of a verified RX 6800 XT (Navi 21) register through the UEFI EFI_PCI_IO_PROTOCOL Mem.Read interface, without GPU ownership changes.

## Preconditions (hard gates)
1. B02 source and binary reviewed; exact image hash and observed physical GPU BAR output recorded. B02 user reported 34 PCI handles, BAR values were not captured.
2. Identify the correct PCI function by vendor 0x1002, device 0x73BF, PCI class and subsystem information; do not select the first AMD function indiscriminately.
3. Determine which BAR represents register MMIO using authoritative Navi 21 documentation or corroborated vendor driver source. BAR numbers and sizes must be measured, not guessed.
4. Identify an exact register offset with independently reviewed semantics showing that reads have no side effects. Record primary source reference, address width, endianness, required power state, and expected behavior.
5. Cross-check EFI_PCI_IO_PROTOCOL Mem.Read ABI (This, Width, BarIndex, Offset, Count, Buffer), GetBarAttributes layout, and UEFI allocation/free semantics with EDK II and UEFI normative documentation. Add compile-time offset and function-pointer signature checks.

## Runtime behavior
- Display B03 banner, firmware identity, PCI BDF and full 16-bit vendor/device IDs.
- Enumerate candidate BARs read-only and validate resource descriptors; refuse ambiguous, malformed or unverified mappings.
- Use only EFI_PCI_IO_PROTOCOL Mem.Read with one allowlisted 32-bit aligned register and Count=1; never dereference a physical BAR pointer directly.
- No polling loops or retries after a failed read; bounded timeout is firmware-dependent and cannot be promised.
- Report EFI_STATUS and register value without interpreting unknown bits as facts.
- Retain keyboard-gated exit, preserve Windows boot behavior and prior B01/B02 backups.

## Explicit prohibitions
No Mem.Write, Io.Write, Pci.Write, SetBarAttributes, Attributes modifications, PCI command register writes, GPU reset, GPU takeover, ROM execution, DMA, firmware variable writes, disk formatting, or changes to Windows boot order.

## Verification / acceptance
1. Independent register-source review with documented offset and read-side-effect analysis.
2. Build strict freestanding x64 UEFI PE32+ with relocations, no imports, ABI assertions.
3. Host-side negative tests for missing PCI protocol, wrong device ID, wrong BAR type, invalid descriptor, misaligned offset, failed EFI Mem.Read, and resource cleanup.
4. QEMU exact-binary boot, console output and clean keyboard exit; QEMU may not emulate RX 6800 XT MMIO, so this is a control-flow test only.
5. Separate physical USB test on Full Metal Bitch only after steps 1–4 pass; preserve B02 rollback image and verify hashes before replacing BOOTX64.EFI.
6. Record exact observed BDF, BAR, register offset, EFI_STATUS, value, exit result, and Windows return in project-memory/EVIDENCE.md.

## Current blocker
The B02 physical report confirmed a successful boot, 34 PCI handles, displayed GPU resource information, and clean Windows return, but exact BAR layout, register aperture and attributes were not captured. No B03 register offset is yet authorized for physical probing.
