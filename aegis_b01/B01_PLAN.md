# Aegis B01 — Read-only UEFI Hardware Discovery
Status: IMPLEMENTATION OPEN
Entry: UEFI USB boot on physical Ryzen 5700G / B550M PG Riptide / RX 6800 XT target.
B00 accepted: physical keyboard-gated boot, SHA256 FA097445D479CA9F425F2CEFFB048FD33FB68B0D0419B3BABDA86B806AD52AD6.
B01 acceptance criteria:
1. Preserve B00 fallback and keyboard-gated exit.
2. Report UEFI firmware identity, memory map total and descriptors, and PCI device vendor/device IDs using firmware interfaces; no privileged direct probing or device configuration writes.
3. Detect AMD vendor 0x1002 GPU, report actual device ID without assuming a particular bus address.
4. Bound all firmware-returned lengths, pointers and console output; handle missing protocols without crashing.
5. Build deterministic PE32+ x86-64 EFI app with relocations and no external DLL imports.
6. Verify QEMU/EDK2 execution, error cases, and keypress behavior before copying to removable USB.
7. Physical target validation only after user has reinserted USB on Main and approved deployment.
Safety: no access to target Windows NVMe, ESP, NVRAM boot entries or firmware setup changes.
B02 candidate: CPU topology and GPU BAR discovery, still read-only.
