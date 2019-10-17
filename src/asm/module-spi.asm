#define SSP_Base  0xFFF8D000
#define SSP_CR0   0x000
#define SSP_CR1   0x004
#define SSP_DR    0x008
#define SSP_SR    0x00C
#define SSP_CPSR  0x010


/*extern void module_SPI_trancieve (uint8_t * tx_data_ptr, uint8_t * rx_data_ptr)*/
.global module_SPI_trancieve
module_SPI_trancieve: /*-----------------------------------------*/
/*
 * R0 - tx datat pointer
 * R1 - number of bytes
 * R2 - ptr from
 * R3 - SSP_Base
 * R4 - work register
 * R5 - number of bytes to write in Page Program command
 * R6 - number of bytes to receive in Page Program command (R5 + 5 due to WREN & PP)
 * R7 - SSP Status Register (SR)
 *
 */

	PUSH {R0-R8, LR}

	LDR  R7, [R3, #SSP_SR] // R7 <= SSP_SR addr

    TST  R7, #0x2 //sets the CPSR Z flag to 0 if result of condition (R7 AND 0x2) is true, sets Z flag to 1 otherwise
    BEQ  RECEIVE_M  // Transmit FIFO is full. If prev instruction result is set CPSR Z flag to ZERO then branch to RECIEVE_M

    CMP  R5, #0 //sets the CPSR Z flag to 0 if number of bytes to send greater than 0
    BEQ  RECEIVE_M

    LDR  R4, [R2], #1
    STR  R4, [R3, #SSP_DR]
/*    ADD  R8, R8, #1 */
/*    STR  R8, [R3, #SSP_DR] */
    SUB  R5, R5, #1




