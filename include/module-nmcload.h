//////////////////////////////////////////////////////////////////////////
//                                                                      //
// nmcload.h -                                                          //
//                                                                      //
//      Load and exchange library function declaration                  //
//      For MC6901 in mini configuration                                //
//      LLS-mini                                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include <stdint.h>
#include <module-base.h>
#include <module-armsc.h>
#include <module-libelf.h>
#include <module-vic.h>



# ifndef MODULE_NMCLOAD_H_
#define  MODULE_NMCLOAD_H_


// Processor descriptor
typedef struct {
	int boardNumber;
	int flag;

	uint32_t localNM_startAddr_NMC;		// 32-bit access address to local NM
	uint32_t localNM_startAddr_ARM;		// 8-bit access address to local NM
	uint32_t localNM_size_NMC;				// 32-bit access size to local NM
	uint32_t localNM_size_ARM;				// 8-bit access size to local NM

	uint32_t sharedNM_startAddr_NMC;			// 32-bit access address to another NM
	uint32_t sharedNM_startAddr_ARM;		// 8-bit access address to another NM
	uint32_t sharedNM_size_NMC;			// 32-bit access size another NM
	uint32_t sharedNM_size_ARM; 			// 8-bit access size to another NM

	// SM array
	uint32_t SM_startAddr_NMC;				// 32-bit access to Shared Memory
	uint32_t SM_startAddr_ARM;					// 8-bit access to Shared Memory
	uint32_t SM_size_NMC;						// 32-bit access size of Shared Memory
	uint32_t SM_size_ARM;						// 8-bit access size of Shared Memory

	// AM array
	uint32_t AM_startAddr_NMC;				// 32-bit access to ARM Memory
	uint32_t AM_startAddr_ARM;					// 8-bit access to ARM Memory
	uint32_t AM_size_NMC;						// 32-bit access size of ARM Memory
	uint32_t AM_size_ARM;						// 8-bit access size of ARM Memory

} module_NMC_descriptor_t;

module_NMC_descriptor_t NMcore1_desc;
module_NMC_descriptor_t NMcore2_desc;

	// Library functions return values.
	// All library functions return result code.
	enum RetValue {
		NMCLOAD_OK       = 0,    // OK.
		NMCLOAD_ERROR    = 1,    // Error.
		NMCLOAD_TIMEOUT  = 2,    // Timeout of wait for operation.
		NMCLOAD_FILE     = 3,    // Can't find file for load.
		NMCLOAD_BADADDRESS = 4   // Bad address ranges in user program.
	} RetValue_t;


	static const int ToARMOff = 0x100; // offset of RegLink
	static const int FromARMOff = 0x101;
	static const int buffAddrOff = 0x200; // buffer offset = RegLink + init size

	static const int ISR_TO_ARM      = 0x10E;
	static const int ISR_FROM_ARM    = 0x10F;

	static const int ReadyForCommand = 4;

	// Command to loader flags.
	//const int MOVE_BLOCK   = 1;        // Move memory block.
	//const int FILL_BLOCK   = 2;        // Fill memory block.
	static const int RUN_PROGRAM  = 4;        // Run user program.
	static const int ANY_COMMAND  = 7;        // Mask is any command set.

	typedef struct {
		uint32_t syncFlag; // Synchro flag.
		uint32_t array_addr; // Sync array address.
		uint32_t array_len; // Sync array length in 32-bit words.
		uint32_t value;  // Sync value.
	} module_NMCLOAD_SynchroBlock_t;

	// Addresses of syncro blocks.
	//const int SyncToARMOff = 0x102;
	////const int SyncFromARMOff = 0x106;
	static const int SyncToARMOff = 0x104;
	static const int SyncFromARMOff = 0x108;

	// период ожидания для функций SyncXXX.
	// 0 - бесконечное ожидание.
	// >0 - ожидание в миллисекундах.
	//static int globalTimeout = 0;
	//Пока только бесконечное ожидание

	typedef enum {
		NMCLOAD_COMMAND_PRINT = 0,
		NMCLOAD_COMMAND_COMMON = 1
	} module_NMCLOAD_commandType_t;


	static const int CommandToArm = 0x114;

	typedef struct {
		uint32_t syncFlag;		// Syncro flag.
		uint32_t commantType;	// Command type value.
		uint32_t bufferAddr;	// Command data buffer address.
		uint32_t bufferLen;		// Command data buffer length in 32-bit words.
		uint32_t handlerStatus;	// ARM handler status values*/
	} module_NMCLOAD_commandBlock_t;


#define NUMBER_OF_HANDLERS 3
typedef int (*command_fhandler_t)(uint32_t bufferAddr, uint32_t bufferLen);
// Command Interrupt handler
//void module_NMCLOAD_commandHandler (void)

//------------------
// Common functions.
//------------------

// Create descriptor for processor number 'procNo' on board.
// Return processor descriptor in variable pointed by 'access'.
// Processor numbers is 0-3.
int module_NMCLOAD_GetBoardDesc(int index, module_NMC_descriptor_t * access);


// Call NM initialization code
int module_NMCLOAD_LoadInitCode(module_NMC_descriptor_t * access, uint32_t addrInitFile);

//---------------------
// Processor functions.
//---------------------

// Load user program on processor and start execution.
int module_NMCLOAD_LoadProgramFile(module_NMC_descriptor_t * access, uint32_t addrProgram);

// Wait nm-programm ending .    НЕ РЕАЛИЗОВАНО!
// load nm-programm result if the pointer 'returnValue' is not NULL.
// timeout in milliseconds for waiting end of nm-programm.
// int PL_WaitEndProgram(PL_Access *access, PL_Word *returnValue,PL_Word timeout);

// Write array in shared memory.
// block    - Pointer to source array in PC memory.
// len      - Size of array in 32-bit words.
// address  - Address of destination array in NMC memory.
int module_NMCLOAD_WriteMemBlock(module_NMC_descriptor_t * access, uint32_t * block,
			uint32_t address, uint32_t len);

// Read array from shared memory.
// block    - Pointer to dest buffer in PC memory.
// len      - Size of array in 32-bit words.
// address  - Address of source array in NMC memory.
int module_NMCLOAD_ReadMemBlock(module_NMC_descriptor_t * access, uint32_t * block,
			uint32_t address, uint32_t len);

// Barrier synchronization with program on board processor.
// value        - value sent to processor.
// returnValue  - value received from processor.
int module_NMCLOAD_Sync(module_NMC_descriptor_t * access, int value, int * returnValue);


// Barrier synchronization with program on board processor.
// Additionally perform two values exchange.
// value       - value sent to processor.
// outAddress  - address sent to processor.
// outLen      - size sent to processor.
// returnValue - value received from processor.
// inAddress   - address received from processor.
// inLen       - size received from processor.
// Values return only if pointers are not NULL.

int module_NMCLOAD_SyncArray(
	module_NMC_descriptor_t * access,     // Processor descriptor.

	int value,              // Value sent to processor.
	uint32_t outAddress,     // Address sent to processor.
	uint32_t outLen,         // Size sent to processor.
	int * returnValue,      // Value received from processor.
	uint32_t * inAddress,    // Address received from processor.
	uint32_t *inLen          // Size received from processor.
	);

// Send interrupt on processor.
int module_NMCLOAD_Interrupt(module_NMC_descriptor_t * access);


int module_NMCLOAD_GetStatus(module_NMC_descriptor_t * access, uint32_t * status);

//Link command function handler with command slot
int module_NMCLOAD_linkHandler (int slot, void * f_handler);

//1-st level interrupt handler for NM-interrupt
void module_NMCLOAD_commandHandler_core1 (void);

	//---------------------
	// Common functions.
	//---------------------
int choose_area(module_NMC_descriptor_t *access, uint32_t address, uint32_t len,
		uint32_t **pDest, int RW);

int check_arm_addr(module_NMC_descriptor_t * access, uint32_t address, uint32_t len, int RW);

#endif  // MODULE_NMCLOAD_H_
