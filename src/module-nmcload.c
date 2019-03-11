/*
 * module-spi.h
 *
 *  Created on: 11 мар. 2019 г.
 *      Author: aleksey
 */

#include <module-libelf.h>
#include <module-nmcload.h>

int module_NMLOAD_GetBoardDesc(int index, PL_Access *access)
{

	if((index != 1) && (index != 2)) {
		return PL_ERROR;
	}

	access->boardNumber = index;
	access->flag = 0;


	switch(index) {
	case 1:
		// NM array
		access->localNM_startAddr_NMC =  NM1B0_START_ADDR_32_BIT;
		access->localNM_size_NMC = NM1_LENGTH_32_BIT;
		access->localNM_startAddr_ARM = NM1B0_START_ADDR;
		access->localNM_size_ARM = NM1_LENGTH;

		access->sharedNM_startAddr_NMC =  NM2B0_START_ADDR_32_BIT;
		access->sharedNM_size_NMC = NM2_LENGTH_32_BIT;
		access->sharedNM_startAddr_ARM =  NM2B0_START_ADDR;
		access->sharedNM_size_ARM = NM2_LENGTH;

		break;
	case 2:
		access->localNM_startAddr_NMC =  NM2B0_START_ADDR_32_BIT;
		access->localNM_size_NMC = NM2_LENGTH_32_BIT;
		access->localNM_startAddr_ARM =  NM2B0_START_ADDR;
		access->localNM_size_ARM = NM2_LENGTH;

		access->sharedNM_startAddr_NMC = NM1B0_START_ADDR_32_BIT;
		access->sharedNM_size_NMC = NM1_LENGTH_32_BIT;
		access->sharedNM_startAddr_ARM =  NM1B0_START_ADDR;
		access->sharedNM_size_ARM = NM1_LENGTH;
		break;
}
	// SM array
	access->SM_startAddr_NMC = SMB0_START_ADDR_32_BIT;
	access->SM_startAddr_ARM = SMB0_START_ADDR;
	access->SM_size_NMC = SMB_LENGTH_32_BIT;
    access->SM_size_ARM= SMB_LENGTH;

	// AM array
	access->AM_startAddr_NMC =  AMB0_START_ADDR_32_BIT;
	access->AM_startAddr_ARM =  AMB0_START_ADDR;
	access->AM_size_NMC = AMB_LENGTH_32_BIT;
	access->AM_size_ARM = AMB_LENGTH;
	return PL_OK;
}

int module_NMLOAD_LoadInitCode(PL_Access * access, PL_Addr addrInitFile)
{
	PL_Word * pSrc, * pDest;
	
	if(access == 0 || addrInitFile == 0)
		return PL_ERROR;

	Elf32_Ehdr * ehdr = (Elf32_Ehdr *) addrInitFile;

	// Check ELF-header
	if(check_Ehdr(ehdr)) return PL_FILE;

	//Check number of
	if(ehdr->e_phnum != 1 || ehdr->e_entry != 0) return PL_FILE;

	// ATT: check address
	Elf32_Phdr * phdr = (Elf32_Phdr *) (addrInitFile + ehdr->e_phoff);
	if(phdr->p_paddr != 0) return PL_FILE;
	
	pDest = (PL_Word *) access->localNM_startAddr_ARM;
	// ATT: check address
	pSrc = (PL_Word *) (addrInitFile + phdr->p_offset);
	int size = (phdr->p_filesz + 3) >> 2;
	int i;
/****DEBUG****/////printf("PL_LoadInitCode 44 0000 pDest="); //printf(pDest); //printf("size="); //printf(size); //printf("\n\r");;
	for(i = 0; i < size; i++)
		*pDest++ = *pSrc++;

	// ATT: check address
	PL_Word * RegLink = (PL_Word *) access->localNM_startAddr_ARM + ToARMOff;
	*RegLink = 0;
	// start processor
	module_NMLOAD_Interrupt( access);

	// wait for init complete
	PL_Word Value;
	// ATT: check const for cycles
	for(i = 0; i < 1000; i++){
		Value = *RegLink;
		if(Value & ReadyForCommand)
			break;
	}

	if((Value & ReadyForCommand) == 0) return PL_ERROR;
	return PL_OK;
}

int module_NMLOAD_LoadProgramFile(PL_Access * access, PL_Addr addrProgram)
{

    //printf("DEBUG: PL_LoadProgramFile Start \n\r");
	if(access == 0 || addrProgram == 0)
		return PL_ERROR;

	//PL_Word status;
	//PL_GetStatus(access, &status);

	// Проверяем состояние загрузчика.
    //printf("DEBUG: PL_LoadProgramFile - Check Status \n\r");
	//if ((status & ReadyForCommand) != ReadyForCommand)
		//return PL_ERROR;

	Elf32_Ehdr * ehdr = (Elf32_Ehdr *) addrProgram;

	// is it program?
	if(check_Ehdr(ehdr)) return PL_FILE;

	PL_Word * startAddr; 
	//PL_Word sharedMemSize;
	startAddr = (PL_Word *) access->localNM_size_ARM;
	//sharedMemSize = access->SM_size_ARM;
	
	SynchroBlock * SyncToARM;
	SynchroBlock * SyncFromARM;

	SyncToARM = (SynchroBlock *)(startAddr + SyncToARMOff);
	SyncFromARM = (SynchroBlock *)(startAddr + SyncFromARMOff);
	PL_Word * ToARMAddr = startAddr + ToARMOff;
	PL_Word * FromARMAddr = startAddr + FromARMOff;

	// ATT: check address
	Elf32_Phdr * phdr = (Elf32_Phdr *) (addrProgram + ehdr->e_phoff);
	PL_Word i, j;
	int res;

	PL_Word * pSrc, * pDest;
	PL_Word size, fullSegmentSize;
	
	// check load address
	for(i = 0; i < ehdr->e_phnum; i++) {
	    if(phdr[i].p_type != PT_LOAD) continue; //Если сегмент не загружаемый, переходим к след.
	    PL_Addr Addr = phdr[i].p_paddr; // Физический адрес загружаемого сегмента
	    unsigned int Full_size = phdr[i].p_memsz >> 2; //Размер загружаемого сегмента в памяти (в 32-разрядных словах)
	   

	    if ((res = choose_area(access, Addr, Full_size, &pDest, 1)) != PL_OK)
		return res;
	}

	// Загружаем программные сегменты.
	// Загрузка идет напрямую.
	for(i = 0; i < ehdr->e_phnum; i++) {
		if(phdr[i].p_type != PT_LOAD) continue;

	    PL_Addr Addr = phdr[i].p_paddr;
	    unsigned int Full_size = phdr[i].p_memsz >> 2;
	    if ((res = choose_area(access, Addr, Full_size, &pDest, 1)) != PL_OK)
			return res;

		size = phdr[i].p_filesz >> 2;

		// ATT: check address
		pSrc = (PL_Word *) (addrProgram + phdr[i].p_offset);

		for(j = 0; j < size; j++)
			*pDest++ = *pSrc++;
		
		// Полный размер сегмента.
		fullSegmentSize = phdr[i].p_memsz >> 2;
		// Заполняем нулями остаток сегмента.
		if(size < fullSegmentSize){
			for(; j < fullSegmentSize; j++)
				*pDest++ = 0;
		}
	}
	// Устанавливаем адрес начала программы

	// и запускаем программу
	SyncFromARM->array_addr = ehdr->e_entry;
/****DEBUG****///SP_dbg("SyncFromARM->array_addr=%lx",SyncFromARM->array_addr);
	*ToARMAddr = 0;
	*FromARMAddr = RUN_PROGRAM;


	return PL_OK;
}

int module_NMLOAD_WriteMemBlock(PL_Access * access, PL_Word * block, PL_Addr address, PL_Word len)
{
	int res;
	PL_Word * pSrc, * pDest;
	PL_Word i;
	if(access == 0 || block == 0) return PL_ERROR;

	if ((res = check_arm_addr(access, (PL_Addr) block, len, 0)) != PL_OK)
		return res;

	if ((res = choose_area(access, address, len, &pDest, 1)) != PL_OK)
		return res;

	pSrc = block;
/****DEBUG****///SP_dbg("Write address=%lx len=%lx pDest=%p", address, len, pDest);
	for(i = 0; i < len; i++)
		*pDest++ = *pSrc++;

	return PL_OK;
}

int module_NMLOAD_ReadMemBlock(PL_Access * access, PL_Word * block, PL_Addr address, PL_Word len)
{
	int res;
	PL_Word * pSrc, * pDest;
	PL_Word i;
	if(access == 0 || block == 0) return PL_ERROR;

	if ((res = check_arm_addr(access, (PL_Addr) block, len, 1)) != PL_OK)
		return res;

	if ((res = choose_area(access, address, len, &pSrc, 0)) != PL_OK)
		return res;

	pDest = block;
	for(i = 0; i < len; i++)
		*pDest++ = *pSrc++;

	return PL_OK;
}

int module_NMLOAD_SyncArray(
	PL_Access * access,     
	int value,
	PL_Addr outAddress,
	PL_Word outLen,
	int * returnValue,
	PL_Addr * inAddress,
	PL_Word * inLen)
{
	if(access == 0) return PL_ERROR;
	
	PL_Word * startAddr = (PL_Word *) access->localNM_startAddr_ARM;

	volatile SynchroBlock * SyncToARM, * SyncFromARM;
	SyncToARM = (SynchroBlock *)(startAddr + SyncToARMOff);
	SyncFromARM = (SynchroBlock *)(startAddr + SyncFromARMOff);

	// Используем бит 0..
	PL_Word syncFlag = 1;

	// Записываем передаваемые значения.
	SyncFromARM->value = value;
	SyncFromARM->array_addr = outAddress;
	SyncFromARM->array_len = outLen;

	PL_Word Value = 0;
	//clock_t wait_time;
	//#define DELTA_wait_time	5000

	// Ожидаем установки своего бита в 1. (Input).
	//if(globalTimeout == 0) {
		// Бесконечное ожидание.
		while((Value & syncFlag) == 0) {
			Value = SyncToARM->syncFlag;
		}

	
	// принимаем данные.
	if(returnValue) {
		*returnValue = SyncToARM->value;
		SyncToARM->value = 0;
	}

	if(inAddress) {
		//*inAddress = SyncToARM->array_addr;
		choose_area(access, SyncToARM->array_addr, SyncToARM->array_len, inAddress, 1);
	}

	if(inLen) {
		*inLen = SyncToARM->array_len;
	}


	// Устанавливаем бит синхронизации в 1 (Output).
	SyncFromARM->syncFlag = syncFlag;

		while(Value & syncFlag) {
			Value = SyncToARM->syncFlag;
		}

	// устанавливаем свой бит в 0.
	SyncFromARM->syncFlag = 0;
	

	return PL_OK;
}

int module_NMLOAD_Sync(
	PL_Access * access,
	int value,
	int * returnValue)
{

	return module_NMLOAD_SyncArray(
		access,
		value,
		0,
		0,
		returnValue,
		0,
		0);
}


int module_NMLOAD_Interrupt(PL_Access * access)
{
	//Generate non-maskable interrupt
	module_ARMSC_NMCnum_t nmc_num;
	switch(access->boardNumber){
		case 0:
			nmc_num = ARMSC_NMC0;
			break;
		case 1:
			nmc_num = ARMSC_NMC1;
			break;
	}
	module_ARMSC_generate_NMU_interrupt(nmc_num, ARMSC_NONMASKABLE_INT);
	return PL_OK;
}

int module_NMLOAD_GetStatus(PL_Access * access, PL_Word * status)
{
	PL_Word * RegLink = (PL_Word *) access->localNM_startAddr_ARM + ToARMOff;
	*status = *RegLink;
	return PL_OK;
}


	//---------------------
	// Common functions.
	//---------------------

int choose_area(PL_Access * access, PL_Addr address, PL_Word len,
		PL_Word * *pDest, int RW)
{
	PL_Addr max_adr;

	// NM array in local mirrored to 0x0 address range
	if (address < access->localNM_size_NMC)
	{
		if((address + len) > access->localNM_size_NMC) return PL_BADADDRESS; //error if array is not fit in NMC bank
		if ((RW == 1) && (address < buffAddrOff))	return PL_BADADDRESS; //error if adress in loader range
		*pDest = (PL_Word *) (access->localNM_size_ARM) + address; //adress in byte
	}
	// NM array in local bank
	else if((address >= access->localNM_startAddr_NMC) &&
			(address < ((access->localNM_startAddr_NMC)+(access->localNM_size_NMC))) )
	{
		max_adr =access->localNM_startAddr_NMC + access->localNM_size_NMC;
		if((address + len) > max_adr) return PL_BADADDRESS;
		*pDest = (PL_Word *) (address * 4); //adress in byte
	}
	// SM array
	else if((address >= access->SM_startAddr_NMC) && (address < ((access->SM_startAddr_NMC)+(access->SM_size_NMC))) )
	{
		max_adr = access->SM_startAddr_NMC + access->SM_size_NMC;
		if((address + len) > max_adr) return PL_BADADDRESS;
		*pDest = (PL_Word *) (address * 4); //adress in byte
	}
	else if((address >= access->AM_startAddr_NMC) && (address < ((access->AM_startAddr_NMC)+(access->AM_size_NMC))) )
	{
	// AM array
		max_adr = access->AM_startAddr_NMC + access->AM_size_NMC;
		if((address + len) > max_adr) return PL_BADADDRESS;
		//if ((RW == 1) && (address < writable_addrAM))	return PL_BADADDRESS;
		if (RW == 1) _assert("Attempt to unsafe write to ARM memory. Address: 0x%X, len: 0x%X",
				(uint32_t) address,(uint32_t) len);
		*pDest = (PL_Word *) (address * 4); //adress in byte

	} else
	{
		_runtime_error("BADADDRESS address=%lx len=%lx",address, len);
		return PL_BADADDRESS;
	}
	return PL_OK;
}

int check_arm_addr(PL_Access * access, PL_Addr address, PL_Word len, int RW)
{
	PL_Addr max_adr;
	PL_Word len_bytes = len * 4;
	//int in_bytes = sizeof(PL_Addr);

	if((address >= (access->localNM_size_ARM)) &&
		  (address < (access->localNM_size_ARM + access->localNM_size_ARM)) ) {
		// NM shared
		max_adr = access->localNM_size_ARM + access->localNM_size_ARM;
		if((address + len_bytes) > max_adr) return PL_BADADDRESS;
		if ((RW == 1) && (address < (access->localNM_size_ARM + buffAddrOff)) )
			return PL_BADADDRESS;

	}
	else if((address >= (access->SM_startAddr_ARM)) &&
		  (address < (access->SM_startAddr_ARM)+(access->SM_size_ARM)) )
	{
		// SM array
		max_adr = access->SM_startAddr_ARM + access->SM_size_ARM;
		if((address + len_bytes) > max_adr) return PL_BADADDRESS;

	}
	else if((address >= (access->AM_startAddr_ARM)) &&
		  (address < (access->AM_startAddr_ARM)+(access->AM_size_ARM) ) )
	{
		// AM array
		max_adr = access->AM_startAddr_ARM + access->AM_size_ARM;
		if((address + len_bytes) > max_adr) return PL_BADADDRESS;
		if (RW == 1) _assert("Attempt to unsafe write to ARM memory. Address: 0x%X, len: 0x%X",
				(uint32_t) address,(uint32_t) len);
//		if ((RW == 1) && (address < (writable_addrAM * in_bytes)) )
//			return PL_BADADDRESS;
	} else
	{
		_runtime_error("BADADDRESS address=%lx len=%lx",address, len);
		return PL_BADADDRESS;
	}
	return PL_OK;
}
