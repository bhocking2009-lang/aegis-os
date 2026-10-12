# Aegis B02 — GPU Resource Discovery (PROPOSED)

## Goal
Extend physically successful B01 into safe GPU resource discovery for the AMD RX 6800 XT (PCI vendor 1002, expected device 73BF). No driver initialization or writes to GPU MMIO registers in this milestone.

## Baseline
B01 physical test user-reported PASS 7/7 on 2026-10-12. Candidate SHA256 65A3D7E7673119061C7B01CA3D4E1FEFF2E809467F7B2FB33EC1CB059B86F081. B01 formal review/regression remain open. Preserve exact B01 image and B00 accepted image.

## Deliverables
1. Read-only PCI GPU identification with vendor/device/subsystem/revision/class codes.
2. Read-only PCI BAR inspection via EFI_PCI_IO_PROTOCOL.GetBarAttributes or PCI configuration reads. Do not size BARs by writing all-ones to registers.
3. Safe resource report: BAR index, address/length when provided by firmware, I/O vs memory and 32/64-bit characteristics, bounded output.
4. Strict device allowlisting for AMD GPU; reject unknown hardware and avoid any MMIO register dereference unless individually specified and validated.
5. ABI compile-time assertions, deterministic build, independent review, QEMU test, physical USB user test, evidence and hash freeze.

## Safety
No NVMe/ESP/partition/NVRAM writes. No GPU reset, power-state transition, PCI configuration writes, DMA setup, MMIO writes, VRAM mapping changes, or driver takeover. Preserve Bravo resident 122B. USB deployment only after source review and verification; independently identify removable media before staging.

## Acceptance
Boots from one-time UEFI USB menu; B01 memory and PCI outputs retained; reports RX 6800 XT vendor/device and firmware-exposed BAR resources without writes; handles absent/unsupported BARs safely; keyboard exit works; no observed impact on Windows installation; exact binary hash recorded; independent review and QEMU evidence completed. If a hardware value cannot be read safely, report unsupported rather than fabricate.

## Scope control
B02 is GPU RESOURCE DISCOVERY, not graphics acceleration, native display takeover, VRAM allocator, or GPU compute. Those require separate design and safety gates.
