/*
 * module-spi.h
 *
 *  Created on: 11 мар. 2019 г.
 *      Author: aleksey
 */

#include <module-nmcload.h>


int module_NMCLOAD_GetBoardDesc(int index, PL_Access *access)
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

int module_NMCLOAD_LoadInitCode(PL_Access * access, uint32_t addrInitFile)
{
	uint32_t * pSrc, * pDest;
	
	if(access == 0 || addrInitFile == 0)
		return PL_ERROR;

	Elf32_Ehdr * elf_header = (Elf32_Ehdr *) addrInitFile;

	// Check ELF-header
	if(check_Ehdr(elf_header)) return PL_FILE;

	//Check number of
	if(elf_header->e_phnum != 1 || elf_header->e_entry != 0) return PL_FILE;

	// ATT: check address
	Elf32_Phdr * program_header = (Elf32_Phdr *) (addrInitFile + elf_header->e_phoff);
	if(program_header->p_paddr != 0) return PL_FILE;
	
	pDest = (uint32_t *) access->localNM_startAddr_ARM;
	// ATT: check address
	pSrc = (uint32_t *) (addrInitFile + program_header->p_offset);
	int size = (program_header->p_filesz + 3) >> 2;
	int i;
/****DEBUG****/////printf("PL_LoadInitCode 44 0000 pDest="); //printf(pDest); //printf("size="); //printf(size); //printf("\n\r");;
	for(i = 0; i < size; i++)
		*pDest++ = *pSrc++;

	// ATT: check address
	uint32_t * RegLink = (uint32_t *) access->localNM_startAddr_ARM + ToARMOff;
	*RegLink = 0;
	// start processor
	module_NMCLOAD_Interrupt( access);
	//module_ARMSC_generate_NMU_interrupt(ARMSC_NMC1, ARMSC_NONMASKABLE_INT);

	// wait for init complete and ReadyForCommand state
	uint32_t Value;
	// ATT: check const for cycles
	for(i = 0; i < 1000; i++){
		Value = *RegLink;
		if(Value & ReadyForCommand)
			break;
	}

	if((Value & ReadyForCommand) == 0) return PL_ERROR;
	return PL_OK;
}


int module_NMCLOAD_Halt(PL_Access * access, uint32_t addrInitFile)
{
	int i;
	uint32_t * RegLink = (uint32_t *) access->localNM_startAddr_ARM + ToARMOff;
	*RegLink = 0;
	uint32_t Value;

	Value = *RegLink;
	if (Value & ReadyForCommand)
	{
		_assert("NMCLOAD: NMC%i is already ready to accept command ",access->boardNumber );
		return PL_OK;
	}
		module_NMCLOAD_Interrupt( access);

	//wait for ReadyForCommand state
	for(i = 0; i < 1000; i++){
		Value = *RegLink;
		if(Value & ReadyForCommand)
			break;
	}
	if((Value & ReadyForCommand) == 0) return PL_ERROR;
	return PL_OK;
}

int module_NMCLOAD_LoadProgramFile(PL_Access * access, uint32_t addrProgram)
{

    //printf("DEBUG: PL_LoadProgramFile Start \n\r");
	if(access == 0 || addrProgram == 0)
		return PL_ERROR;

	uint32_t status;
	module_NMCLOAD_GetStatus(access, &status);

	// Проверяем состояние загрузчика.
    //printf("DEBUG: PL_LoadProgramFile - Check Status \n\r");
	if ((status & ReadyForCommand) != ReadyForCommand)
	{
		_runtime_error("NMCLOAD: NMC loader has non-ready status");
		return PL_ERROR;
	}

	Elf32_Ehdr * ehdr = (Elf32_Ehdr *) addrProgram;

	// is it program?
	if(check_Ehdr(ehdr)) return PL_FILE;

	uint32_t * startAddr;
	//PL_Word sharedMemSize;
	startAddr = (uint32_t *) access->localNM_startAddr_ARM;
	//sharedMemSize = access->SM_size_ARM;
	
	volatile SynchroBlock * SyncToARM;
	volatile SynchroBlock * SyncFromARM;

	SyncToARM = (SynchroBlock *)(startAddr + SyncToARMOff);
	SyncFromARM = (SynchroBlock *)(startAddr + SyncFromARMOff);
	volatile uint32_t * ToARMAddr = startAddr + ToARMOff;
	volatile uint32_t * FromARMAddr = startAddr + FromARMOff;

	// ATT: check address
	Elf32_Phdr * phdr = (Elf32_Phdr *) (addrProgram + ehdr->e_phoff);
	uint32_t i, j;
	int res;

	uint32_t * pSrc, * pDest;
	uint32_t size, fullSegmentSize;
	
	// check load address
	for(i = 0; i < ehdr->e_phnum; i++) {
	    if(phdr[i].p_type != PT_LOAD) continue; //Если сегмент не загружаемый, переходим к след.
	    uint32_t Addr = phdr[i].p_paddr; // Физический адрес загружаемого сегмента
	    unsigned int Full_size = phdr[i].p_memsz >> 2; //Размер загружаемого сегмента в памяти (в 32-разрядных словах)
	   

	    if ((res = choose_area(access, Addr, Full_size, &pDest, 1)) != PL_OK)
		return res;
	}

	// Загружаем программные сегменты.
	// Загрузка идет напрямую.
	for(i = 0; i < ehdr->e_phnum; i++) {
		if(phdr[i].p_type != PT_LOAD) continue;

	    uint32_t Addr = phdr[i].p_paddr;
	    unsigned int Full_size = phdr[i].p_memsz >> 2;
	    if ((res = choose_area(access, Addr, Full_size, &pDest, 1)) != PL_OK)
			return res;

		size = phdr[i].p_filesz >> 2;

		// ATT: check address
		pSrc = (uint32_t *) (addrProgram + phdr[i].p_offset);

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

	while (!(*ToARMAddr==2))
	{

	}
	//_debug ("FromARMAddr value = 0x%X", *FromARMAddr);
	//_debug ("ToARMAddr value = 0x%X", *ToARMAddr);

	return PL_OK;
}

int module_NMCLOAD_WriteMemBlock(PL_Access * access, uint32_t * block, uint32_t address, uint32_t len)
{
	int res;
	uint32_t * pSrc, * pDest;
	uint32_t i;
	if(access == 0 || block == 0) return PL_ERROR;

	if ((res = check_arm_addr(access, (uint32_t) block, len, 0)) != PL_OK)
		return res;

	if ((res = choose_area(access, address, len, &pDest, 1)) != PL_OK)
		return res;

	pSrc = block;
/****DEBUG****///SP_dbg("Write address=%lx len=%lx pDest=%p", address, len, pDest);
	for(i = 0; i < len; i++)
		*pDest++ = *pSrc++;

	return PL_OK;
}

int module_NMCLOAD_ReadMemBlock(PL_Access * access, uint32_t * block, uint32_t address, uint32_t len)
{
	int res;
	uint32_t * pSrc, * pDest;
	uint32_t i;
	if(access == 0 || block == 0) return PL_ERROR;

	if ((res = check_arm_addr(access, (uint32_t) block, len, 1)) != PL_OK)
		return res;

	if ((res = choose_area(access, address, len, &pSrc, 0)) != PL_OK)
		return res;

	pDest = block;
	for(i = 0; i < len; i++)
		*pDest++ = *pSrc++;

	return PL_OK;
}

int module_NMCLOAD_SyncArray(
	PL_Access * access,     
	int value,
	uint32_t outAddress,
	uint32_t outLen,
	int * returnValue,
	uint32_t * inAddress,
	uint32_t * inLen)
{
	if(access == 0) return PL_ERROR;
	
	uint32_t * startAddr = (uint32_t *) access->localNM_startAddr_ARM;

	volatile SynchroBlock * SyncToARM, * SyncFromARM;
	SyncToARM = (SynchroBlock *)(startAddr + SyncToARMOff);
	SyncFromARM = (SynchroBlock *)(startAddr + SyncFromARMOff);

	// Используем бит 0..
	uint32_t syncFlag = 1;

	// Записываем передаваемые значения.
	SyncFromARM->value = value;
	SyncFromARM->array_addr = outAddress;
	SyncFromARM->array_len = outLen;

	uint32_t Value = 0;
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

int module_NMCLOAD_Sync(
	PL_Access * access,
	int value,
	int * returnValue)
{

	return module_NMCLOAD_SyncArray(
		access,
		value,
		0,
		0,
		returnValue,
		0,
		0);
}


int module_NMCLOAD_Interrupt(PL_Access * access)
{
	//Generate non-maskable interrupt
	module_ARMSC_NMCnum_t nmc_num;
	uint32_t value = 0;
	switch(access->boardNumber){
		case 1:
			nmc_num = ARMSC_NMC1;
			value = 1 << 4;
			break;
		case 2:
			nmc_num = ARMSC_NMC2;
			value = 1 << 5;
			break;
	}
	module_ARMSC_generate_NMU_interrupt(nmc_num, ARMSC_NONMASKABLE_INT);
	//PL_Word * const CtrlNMURegAddr = (PL_Word *)0x0fff81038;
	//*CtrlNMURegAddr = value;
	return PL_OK;
}

int module_NMCLOAD_GetStatus(PL_Access * access, uint32_t * status)
{
	uint32_t * RegLink = (uint32_t *) access->localNM_startAddr_ARM + ToARMOff;
	*status = *RegLink;
	return PL_OK;
}


	//---------------------
	// Common functions.
	//---------------------

int choose_area(PL_Access * access, uint32_t address, uint32_t len,
		uint32_t * *pDest, int RW)
{
	uint32_t max_adr;

	// NM array in local mirrored to 0x0 address range
	if (address < access->localNM_size_NMC)
	{
		if((address + len) > access->localNM_size_NMC) return PL_BADADDRESS; //error if array is not fit in NMC bank
		if ((RW == 1) && (address < buffAddrOff))	return PL_BADADDRESS; //error if adress in loader range
		*pDest = (uint32_t *) (access->localNM_startAddr_ARM) + address; //adress in byte
	}
	// NM array in local bank
	else if((address >= access->localNM_startAddr_NMC) &&
			(address < ((access->localNM_startAddr_NMC)+(access->localNM_size_NMC))) )
	{
		max_adr =access->localNM_startAddr_NMC + access->localNM_size_NMC;
		if((address + len) > max_adr) return PL_BADADDRESS;
		*pDest = (uint32_t *) (address * 4); //adress in byte
	}
	// SM array
	else if((address >= access->SM_startAddr_NMC) && (address < ((access->SM_startAddr_NMC)+(access->SM_size_NMC))) )
	{
		max_adr = access->SM_startAddr_NMC + access->SM_size_NMC;
		if((address + len) > max_adr) return PL_BADADDRESS;
		*pDest = (uint32_t *) (address * 4); //adress in byte
	}
	else if((address >= access->AM_startAddr_NMC) && (address < ((access->AM_startAddr_NMC)+(access->AM_size_NMC))) )
	{
	// AM array
		max_adr = access->AM_startAddr_NMC + access->AM_size_NMC;
		if((address + len) > max_adr) return PL_BADADDRESS;
		//if ((RW == 1) && (address < writable_addrAM))	return PL_BADADDRESS;
		if (RW == 1) _assert("Attempt to unsafe write to ARM memory. Address: 0x%X, len: 0x%X",
				(uint32_t) address,(uint32_t) len);
		*pDest = (uint32_t *) (address * 4); //adress in byte

	} else
	{
		_runtime_error("BADADDRESS address=%lx len=%lx",address, len);
		return PL_BADADDRESS;
	}
	return PL_OK;
}

int check_arm_addr(PL_Access * access, uint32_t address, uint32_t len, int RW)
{
	uint32_t max_adr;
	uint32_t len_bytes = len * 4;
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
