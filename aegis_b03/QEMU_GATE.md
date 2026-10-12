# B03 QEMU test gate — 2026-10-12

Candidate SHA256: C96C4DA65E040AA7C3223B7C4F85CB82D710CBD8B47871CFEB32E117EEA7CCEF.

## Executed
- Built using GCC with `-Werror`; verified PE32+ EFI application, relocation directory, and no external DLL imports: PASS.
- Launched qemu-system-x86_64 with OVMF pflash, q35, 512 MiB RAM, USB keyboard, VGA, and isolated FAT directory containing B03 BOOTX64.EFI: PASS (QEMU process remained running).
- Captured first screen with HMP screendump: 1280x800.
- Injected `sendkey spc`, captured second screen: 1280x800, 72,886 changed pixels: keyboard injection and screen transition observed, semantic pagination not yet proven.
- Injected `sendkey q`, captured third screen: 640x480; significant screen transition consistent with exiting UEFI application to firmware, but exact firmware state not independently decoded.

## Gate decision: HOLD (not accepted for physical deployment)
- Screen differences alone cannot prove that pager text, per-page contents, and Q semantics are correct.
- No verified full transcript capture or USB-only write path exists.
- Pager uses fixed 20-line threshold rather than QueryMode, and direct output of long vendor strings may wrap unpredictably.
- No B03 MMIO reads are enabled or authorized.
- Do not replace physically accepted B02 USB binary.

## Next checks
1. Obtain legible QEMU screenshots or serial/text capture and verify expected B03 banner, pagination prompt, complete PCI entries, and Q exit.
2. Test multiple pages, no keyboard, Q during paging, and end-of-output behavior.
3. Add opt-in, strictly verified boot-USB logging with negative tests against non-USB volumes.
4. Only after independent review and passing gates stage a separate B03 physical test candidate.
