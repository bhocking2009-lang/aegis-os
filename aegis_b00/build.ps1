# Aegis B00 build on Windows MSYS2 UCRT64
$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
$gcc = 'C:\msys64\ucrt64\bin\gcc.exe'
if (!(Test-Path $gcc)) { throw 'Missing MSYS2 UCRT64 GCC' }
& $gcc -O2 -ffreestanding -fno-stack-protector -fno-builtin -mno-red-zone -nostdlib '-Wl,--subsystem,10' '-Wl,--entry,efi_main' '-Wl,--image-base,0x10000000' '-Wl,--enable-reloc-section' -o aegis_b00.efi main.c
if ($LASTEXITCODE -ne 0) { throw "Compiler exit $LASTEXITCODE" }
Get-FileHash .\aegis_b00.efi -Algorithm SHA256
