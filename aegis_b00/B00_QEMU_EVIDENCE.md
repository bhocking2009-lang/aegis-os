# Aegis B00 — Firmware execution gate evidence
Date: 2026-10-12
Status: QEMU/EDK2 UEFI boot PASS. Physical target not tested.
Host: Main DESKTOP-EUVKRHL. Emulator: QEMU 11.1.0; firmware: edk2-x86_64-code.fd; machine q35; 512MB RAM; 1 vCPU.
Boot media: isolated directory-backed virtual FAT volume under this project workspace. Only project-local files used; no physical disk attached.
Boot file: vmfat/EFI/BOOT/BOOTX64.EFI copied from aegis_b00.efi.
Serial evidence: qemu-serial.log; emulator stderr: qemu-error.log.
Observed serial:
AEGIS B00
Firmware vendor: EDK II
Firmware revision: 0x00010000
After application return, firmware loaded UiApp Boot0000 (normal firmware boot manager). No observed crash or hang.
QEMU was stopped by test harness after 12 seconds; no claim of guest shutdown.
Latest build artifact SHA256: 80B1A42C8A3F0FD42FF7C7C7CD1C9ECD0C53E0BE71F6910D5EB4D83FB1A66A70
PE validation: AMD64 0x8664; PE32+; EFI application subsystem 10; base relocations present; no imported DLL names.
Remaining gates: independent code/ABI review, confirm USB packaging, controlled physical boot on target with explicit user approval. Do not modify target Windows NVMe, ESP or boot entries.
