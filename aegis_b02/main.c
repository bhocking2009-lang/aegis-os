#include "efi_stub.h"
#include "../project-memory/abi_assertions.h"
/* Aegis B01: read-only firmware inventory, keyboard-gated exit. */
#define OUT(s) out->OutputString(out, (const CHAR16 *)u##s)
/* Keep a real absolute pointer in the PE image so UEFI relocation is testable. */
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *st);
static EFI_STATUS (EFIAPI *volatile aegis_entry_anchor)(EFI_HANDLE, EFI_SYSTEM_TABLE *) = efi_main;
static void dec(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out, uint64_t n) {
  CHAR16 b[24]; int i=23; b[i--]=0;
  do { b[i--]=(CHAR16)('0'+(n%10)); n/=10; } while(n && i>=0);
  out->OutputString(out,&b[i+1]);
}
static void hex(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out, uint64_t n, int digits) {
  static const CHAR16 h[]={ '0','1','2','3','4','5','6','7',
                             '8','9','A','B','C','D','E','F' };
  CHAR16 b[19]; int i; if(digits<1) digits=1; if(digits>16) digits=16;
  for(i=digits-1;i>=0;--i){ b[i]=h[n&15U]; n>>=4; } b[digits]=0;
  out->OutputString(out,b);
}
static void vendor(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out, const CHAR16 *s) {
  CHAR16 b[81]; UINTN i;
  if(!s) { OUT("(not supplied)"); return; }
  for(i=0;i<80;i++){ CHAR16 c=s[i]; if(!c) break; b[i]=c; }
  b[i]=0; out->OutputString(out,b);
  if(i==80) OUT("...");
}
static void memory_map(EFI_BOOT_SERVICES *bs, EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out) {
  EFI_MEMORY_DESCRIPTOR *map=0;
  UINTN needed=0,key=0,stride=0,alloc=0,i,entries;
  uint32_t version=0;
  EFI_STATUS st=bs->GetMemoryMap(&needed,0,&key,&stride,&version);
  if(st!=EFI_BUFFER_TOO_SMALL) { OUT("GetMemoryMap probe error: ");hex(out,st,16);OUT("\r\n");return; }
  if(stride<sizeof(EFI_MEMORY_DESCRIPTOR) || stride>4096) {
    OUT("Invalid descriptor stride\r\n");return;
  }
  for(unsigned attempt=0;attempt<5;attempt++) {
    UINTN margin=8*stride+1024;
    if(needed>8*1024*1024-margin) { OUT("Memory map exceeds 8MiB limit\r\n");return; }
    alloc=needed+margin;
    st=bs->AllocatePool(EfiLoaderData,alloc,(void **)&map);
    if(EFI_ERROR(st) || !map){OUT("AllocatePool error: ");hex(out,st,16);OUT("\r\n");return;}
    UINTN map_bytes=alloc;
    st=bs->GetMemoryMap(&map_bytes,map,&key,&stride,&version);
    if(st==EFI_BUFFER_TOO_SMALL) {
      EFI_STATUS free_st=bs->FreePool(map);map=0;
      if(EFI_ERROR(free_st)){OUT("FreePool failed\r\n");return;}
      needed=map_bytes;
      if(stride<sizeof(EFI_MEMORY_DESCRIPTOR)||stride>4096){OUT("Bad stride on retry\r\n");return;}
      continue;
    }
    if(EFI_ERROR(st)){OUT("GetMemoryMap error: ");hex(out,st,16);OUT("\r\n");bs->FreePool(map);return;}
    if(stride<sizeof(EFI_MEMORY_DESCRIPTOR)||stride>4096 || map_bytes>alloc || map_bytes%stride) {
      OUT("Malformed memory map\r\n");bs->FreePool(map);return;
    }
    entries=map_bytes/stride;
    OUT("Descriptor count: ");dec(out,entries);
    OUT("  Stride: ");dec(out,stride);
    OUT("  Version: ");dec(out,version);OUT("\r\n");
    uint64_t pages=0,usable=0;
    for(i=0;i<entries;i++){
      const EFI_MEMORY_DESCRIPTOR *d=(const EFI_MEMORY_DESCRIPTOR *)((const uint8_t *)map+i*stride);
      if(UINT64_MAX-pages>=d->NumberOfPages) pages+=d->NumberOfPages;
      if(d->Type==EfiConventionalMemory && UINT64_MAX-usable>=d->NumberOfPages) usable+=d->NumberOfPages;
      if(i<32){
        OUT("  [");dec(out,i);OUT("] type ");dec(out,d->Type);
        OUT(" start 0x");hex(out,d->PhysicalStart,16);
        OUT(" pages ");dec(out,d->NumberOfPages);OUT("\r\n");
      }
    }
    if(entries>32) OUT("  (remaining descriptors omitted from console)\r\n");
    OUT("Total described pages: ");dec(out,pages);OUT("\r\n");
    OUT("Conventional pages: ");dec(out,usable);OUT("\r\n");
    bs->FreePool(map);
    return;
  }
  OUT("Memory map changed too often (5 retries)\r\n");
}
static void pci_inventory(EFI_BOOT_SERVICES *bs, EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out) {
  EFI_HANDLE *handles=0;
  UINTN count=0,i,amd=0,listed=0;
  EFI_STATUS st=bs->LocateHandleBuffer(EFI_BY_PROTOCOL,&AEGIS_PCI_IO_GUID,0,&count,&handles);
  if(st==EFI_NOT_FOUND){OUT("No PCI I/O handles found\r\n");return;}
  if(EFI_ERROR(st)){OUT("PCI enumeration error: ");hex(out,st,16);OUT("\r\n");return;}
  if(count && !handles){OUT("PCI handle list missing\r\n");return;}
  OUT("PCI handles: ");dec(out,count);OUT("\r\n");
  for(i=0;i<count && i<256;i++) {
    EFI_PCI_IO_PROTOCOL *pci=0;
    st=bs->HandleProtocol(handles[i],&AEGIS_PCI_IO_GUID,(void **)&pci);
    if(EFI_ERROR(st)||!pci||!pci->Pci.Read) continue;
    uint32_t id=0;
    st=pci->Pci.Read(pci,EFI_PCI_WIDTH_UINT32,0,1,&id);
    if(EFI_ERROR(st)) continue;
    uint16_t ven=(uint16_t)id,dev=(uint16_t)(id>>16);
    if(ven==0xffff) continue;
    UINTN seg=0,bus=0,device=0,func=0;
    EFI_STATUS loc=pci->GetLocation?pci->GetLocation(pci,&seg,&bus,&device,&func):EFI_UNSUPPORTED;
    OUT("  "); if(!EFI_ERROR(loc)) {
      dec(out,seg);OUT(":");hex(out,bus,2);OUT(":");hex(out,device,2);OUT(".");dec(out,func);
    } else OUT("?:??:??.?");
    OUT(" vendor 0x");hex(out,ven,4);OUT(" device 0x");hex(out,dev,4);OUT("\r\n");
    listed++;if(ven==0x1002)amd++;
    if(ven==0x1002 && dev==0x73BF) {
      uint32_t identity=0, classrev=0;
      if(!EFI_ERROR(pci->Pci.Read(pci,EFI_PCI_WIDTH_UINT32,0x2c,1,&identity)) &&
         !EFI_ERROR(pci->Pci.Read(pci,EFI_PCI_WIDTH_UINT32,8,1,&classrev))) {
        OUT("    AMD RX 6800 XT subsystem vendor 0x");hex(out,identity&0xffff,4);
        OUT(" subsystem device 0x");hex(out,identity>>16,4);
        OUT(" class/revision 0x");hex(out,classrev,8);OUT("\r\n");
      }
      for(uint8_t bar=0;bar<6;bar++) {
        uint32_t raw=0;
        EFI_STATUS rd=pci->Pci.Read(pci,EFI_PCI_WIDTH_UINT32,0x10U+(uint32_t)bar*4U,1,&raw);
        if(EFI_ERROR(rd)){OUT("    BAR read failed: ");hex(out,rd,16);OUT("\r\n");break;}
        OUT("    BAR");dec(out,bar);OUT(" raw 0x");hex(out,raw,8);
        if(raw&1U){OUT(" I/O base 0x");hex(out,raw&~3U,8);}
        else {
          uint32_t kind=(raw>>1)&3U;
          OUT(" memory");if(raw&8U)OUT(" prefetchable");
          if(kind==2U && bar<5){
            uint32_t hi=0;EFI_STATUS rh=pci->Pci.Read(pci,EFI_PCI_WIDTH_UINT32,0x14U+(uint32_t)bar*4U,1,&hi);
            if(!EFI_ERROR(rh)){OUT(" 64-bit base 0x");hex(out,((uint64_t)hi<<32)|(raw&~15U),16);}
            else {OUT(" 64-bit high read error ");hex(out,rh,16);}
          } else if(kind==0U){OUT(" 32-bit base 0x");hex(out,raw&~15U,8);}
          else OUT(" reserved/unsupported BAR encoding");
        }
        if(pci->GetBarAttributes){
          uint64_t supports=0;void *resources=0;
          EFI_STATUS attr=pci->GetBarAttributes(pci,bar,&supports,&resources);
          OUT(" attr status 0x");hex(out,attr,16);
          if(!EFI_ERROR(attr)){OUT(" supports 0x");hex(out,supports,16);}
          if(resources && bs->FreePool){EFI_STATUS fs=bs->FreePool(resources);if(EFI_ERROR(fs))OUT(" resource FreePool failed");}
        }
        OUT("\r\n");
        if(!(raw&1U) && ((raw>>1)&3U)==2U && bar<5)bar++;
      }
    }
  }
  if(count>256) OUT("PCI list truncated to first 256 handles\r\n");
  OUT("PCI devices displayed: ");dec(out,listed);
  OUT(" AMD(1002): ");dec(out,amd);OUT("\r\n");
  if(handles){st=bs->FreePool(handles);if(EFI_ERROR(st))OUT("PCI handle list FreePool failed\r\n");}
}
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *st) {
  (void)image;
  if(aegis_entry_anchor != efi_main) return EFI_INVALID_PARAMETER;
  if(!st || !st->ConOut || !st->ConOut->OutputString || !st->BootServices)
    return EFI_INVALID_PARAMETER;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out=st->ConOut;
  EFI_BOOT_SERVICES *bs=st->BootServices;
  OUT("AEGIS B02 - GPU PCI resource discovery\r\n");
  OUT("Firmware vendor: ");vendor(out,st->FirmwareVendor);OUT("\r\n");
  OUT("Firmware revision: 0x");hex(out,st->FirmwareRevision,8);OUT("\r\n");
  if(bs->GetMemoryMap && bs->AllocatePool && bs->FreePool) memory_map(bs,out);
  else OUT("Missing memory map boot services\r\n");
  if(bs->LocateHandleBuffer && bs->HandleProtocol && bs->FreePool) pci_inventory(bs,out);
  else OUT("Missing PCI enumeration boot services\r\n");
  OUT("Press a key to exit\r\n");
  if(st->ConIn && st->ConIn->ReadKeyStroke) {
    EFI_INPUT_KEY key;
    if(st->ConIn->WaitForKey && bs->WaitForEvent) {
      EFI_EVENT e=st->ConIn->WaitForKey;UINTN index=0;
      for(;;){if(st->ConIn->ReadKeyStroke(st->ConIn,&key)==EFI_SUCCESS)break;
        if(EFI_ERROR(bs->WaitForEvent(1,&e,&index)))break;}
    } else {
      for(;;) if(st->ConIn->ReadKeyStroke(st->ConIn,&key)==EFI_SUCCESS)break;
    }
  } else OUT("Keyboard input unavailable\r\n");
  return EFI_SUCCESS;
}

