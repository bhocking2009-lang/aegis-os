#include <stdint.h>
#include <stddef.h>
#if defined(__x86_64__)
#define EFIAPI __attribute__((ms_abi))
#else
#error B00 requires x86_64
#endif
typedef uint64_t EFI_STATUS;
typedef void *EFI_HANDLE;
typedef uint16_t CHAR16;
typedef struct { uint64_t Signature; uint32_t Revision; uint32_t HeaderSize; uint32_t CRC32; uint32_t Reserved; } EFI_TABLE_HEADER;
typedef struct { uint32_t ScanCode; CHAR16 UnicodeChar; } EFI_INPUT_KEY;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL { void *Reset; EFI_STATUS (EFIAPI *ReadKeyStroke)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL*, EFI_INPUT_KEY*); void *WaitForKey; };
struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
 void *Reset;
 EFI_STATUS (EFIAPI *OutputString)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*,CHAR16*);
 void *TestString; void *QueryMode; void *SetMode; void *SetAttribute; void *ClearScreen; void *SetCursorPosition; void *EnableCursor; void *Mode;
};
typedef struct {
 EFI_TABLE_HEADER Hdr;
 CHAR16 *FirmwareVendor;
 uint32_t FirmwareRevision;
 EFI_HANDLE ConsoleInHandle;
 void *ConIn;
 EFI_HANDLE ConsoleOutHandle;
 EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
 EFI_HANDLE StandardErrorHandle;
 EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
 void *RuntimeServices;
 void *BootServices;
 size_t NumberOfTableEntries;
 void *ConfigurationTable;
} EFI_SYSTEM_TABLE;
static CHAR16 banner[] = {'A','E','G','I','S',' ','B','0','0',13,10,0};
static void * __attribute__((used)) reloc_anchor = (void*)banner;
static CHAR16 vendor_prefix[] = {'F','i','r','m','w','a','r','e',' ','v','e','n','d','o','r',':',' ',0};
static CHAR16 newline[] = {13,10,0};
static CHAR16 rev_prefix[] = {'F','i','r','m','w','a','r','e',' ','r','e','v','i','s','i','o','n',':',' ',0};
static CHAR16 prompt[] = {'P','r','e','s','s',' ','a','n','y',' ','k','e','y',' ','t','o',' ','r','e','t','u','r','n',' ','t','o',' ','f','i','r','m','w','a','r','e','.',13,10,0};
static CHAR16 hex_digits[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *st) {
 (void)image;
 if (!st || !st->ConOut || !st->ConOut->OutputString) return 2;
 EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out = st->ConOut;
 out->OutputString(out,banner);
 out->OutputString(out,vendor_prefix);
 if (st->FirmwareVendor) out->OutputString(out,st->FirmwareVendor);
 out->OutputString(out,newline);
 out->OutputString(out,rev_prefix);
 CHAR16 hex[11];hex[0]='0';hex[1]='x';
 for (int i=0;i<8;i++)hex[2+i]=hex_digits[(st->FirmwareRevision >> (28-4*i)) & 15];
 hex[10]=0;out->OutputString(out,hex);out->OutputString(out,newline);
 out->OutputString(out,prompt);
 EFI_SIMPLE_TEXT_INPUT_PROTOCOL *in = (EFI_SIMPLE_TEXT_INPUT_PROTOCOL*)st->ConIn;
 if (!in || !in->ReadKeyStroke) return 0;
 EFI_INPUT_KEY key;
 for (;;) { EFI_STATUS r = in->ReadKeyStroke(in,&key); if (r == 0) break; __asm__ __volatile__("pause"); }
 return 0;
}
