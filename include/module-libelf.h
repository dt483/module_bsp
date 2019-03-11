
#ifndef MODULE_LIBELF_
#define MODULE_LIBELF_

// 		32-Bit Data Types
//							Name 			Size 	Alignment 		Purpose
typedef unsigned long       Elf32_Addr;     // 4 	4 				Unsigned program address
typedef unsigned short int  Elf32_Half;     // 2 	2 				Unsigned medium integer
typedef unsigned long       Elf32_Off;      // 4 	4 				Unsigned file offset
typedef long                Elf32_Sword;    // 4 	4 				Signed large integer
typedef unsigned long       Elf32_Word;     // 4 	4 				Unsigned large integer

//		ELF Header
// 		e_machine: This member’s value specifies the required architecture for an individual file.
const Elf32_Half EM_NEUROCORE01 = 18; // NM6405 in Linux SDK
const Elf32_Half EM_NEUROCORE02 = 64050; //real type NM6405 in Windows SDK
const Elf32_Half EM_NEUROCORE03 = 64051; // SoC

//		e_type: This member identifies the object file type
const Elf32_Half ET_EXEC = 2; // Executable file

// 		ELF Identification
// 		e_ident[ ]: Identification Indexes
//
const int EI_MAG0   = 0; 	// File identification
const int EI_MAG1   = 1; 	// File identification
const int EI_MAG2   = 2; 	// File identification
const int EI_MAG3   = 3; 	// File identification
const int EI_CLASS  = 4; 	// File class
const int EI_DATA   = 5; 	// Data encoding
const int EI_VERSION= 6; 	// File version
const int EI_PAD    = 7; 	// Start of padding bytes
//const int EI_NIDENT = 16; 	//Size of e_ident[]
#define	EI_NIDENT	16


    // A file’s first 4 bytes hold a "magic number", identifying the file as an ELF object file
const unsigned char ELFMAG0 = '\x7f';
const unsigned char ELFMAG1 = 'E';
const unsigned char ELFMAG2 = 'L';
const unsigned char ELFMAG3 = 'F';



typedef struct _Elf32_Ehdr {
        unsigned char   e_ident[EI_NIDENT];
        Elf32_Half      e_type;
        Elf32_Half      e_machine;
        Elf32_Word      e_version;
        Elf32_Addr      e_entry;
        Elf32_Off       e_phoff;
        Elf32_Off       e_shoff;
        Elf32_Word      e_flags;
        Elf32_Half      e_ehsize;
        Elf32_Half      e_phentsize;
        Elf32_Half      e_phnum;
        Elf32_Half      e_shentsize;
        Elf32_Half      e_shnum;
        Elf32_Half      e_shstrndx;
} Elf32_Ehdr;

typedef struct _Elf32_Phdr {
	Elf32_Word  p_type;
	Elf32_Off   p_offset;
	Elf32_Addr  p_vaddr;
	Elf32_Addr  p_paddr;
	Elf32_Word  p_filesz;
	Elf32_Word  p_memsz;
	Elf32_Word  p_flags;
	Elf32_Word  p_align;
} Elf32_Phdr;

const Elf32_Word PT_LOAD = 1;

// simple check for ELF, exec, NMC
int check_Ehdr(Elf32_Ehdr * ehdr)
{
	if(ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
		ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
		ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
		ehdr->e_ident[EI_MAG3] != ELFMAG3)
	{
		return 1;
	}
	if(ehdr->e_type != ET_EXEC ||
		(ehdr->e_machine != EM_NEUROCORE01 &&
			ehdr->e_machine != EM_NEUROCORE02 && 
			ehdr->e_machine != EM_NEUROCORE03) ||
		ehdr->e_phnum == 0)
	{
		return 2;
	}
	return 0;
}

#endif // MODULE_LIBELF_
