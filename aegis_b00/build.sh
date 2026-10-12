#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
: "${CC:=/ucrt64/bin/gcc}"
"$CC" -O2 -ffreestanding -fno-stack-protector -fno-builtin -mno-red-zone -nostdlib -Wl,--subsystem,10 -Wl,--entry,efi_main -Wl,--image-base,0 -o aegis_b00.efi main.c
file aegis_b00.efi || true
sha256sum aegis_b00.efi
