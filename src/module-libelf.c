/*
 * module-libelf.c
 *
 *  Created on: 12 мар. 2019 г.
 *      Author: aleksey
 */

#include <module-libelf.h>

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
