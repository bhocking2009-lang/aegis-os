$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
$gcc = 'C:\msys64\ucrt64\bin\gcc.exe'
$objdump = 'C:\msys64\ucrt64\bin\objdump.exe'
if (!(Test-Path $gcc) -or !(Test-Path $objdump)) { throw 'Required UCRT64 compiler tools unavailable' }
$flags = @(
  '-std=c11','-Wall','-Wextra','-Werror','-ffreestanding',
  '-fno-stack-protector','-mno-red-zone','-nostdlib',
  '-Wl,--subsystem,10','-Wl,--entry,efi_main',
  '-Wl,--enable-reloc-section','-Wl,--dynamicbase',
  '-Wl,--no-insert-timestamp',
  'main.c','-o','aegis_b01_candidate.efi'
)
& $gcc @flags
if ($LASTEXITCODE -ne 0) { throw "GCC failed: $LASTEXITCODE" }
$pe = (& $objdump -p 'aegis_b01_candidate.efi') -join "`n"
if ($LASTEXITCODE -ne 0) { throw 'objdump failed' }
if ($pe -notmatch 'Magic\s+020b') { throw 'Not PE32+' }
if ($pe -notmatch 'Subsystem\s+0000000a') { throw 'Not EFI application subsystem' }
if ($pe -notmatch 'Entry 5\s+[0-9a-fA-F]+\s+(?!00000000)[0-9a-fA-F]+\s+Base Relocation Directory') {
  throw 'Missing PE base relocation directory'
}
if ($pe -match 'DLL Name:') { throw 'Unexpected external DLL import' }
$hash = (Get-FileHash 'aegis_b01_candidate.efi' -Algorithm SHA256).Hash
Write-Output "B01 candidate compiled; PE32+ EFI subsystem 10; relocations present; no DLL names; SHA256=$hash"
Write-Output 'NOT DEPLOYED. QEMU and independent review are separate acceptance gates.'
