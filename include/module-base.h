#ifndef MODULE_BASE_H
#define MODULE_BASE_H


#define DEBUG

	/*Peripheral macro defs*/
#define PERIPHERAL_BASE     ( 0xFFF80000UL                )
#define CRG11_BASE			( PERIPHERAL_BASE + 0UL       )
#define ARMSC_BASE			( PERIPHERAL_BASE + 0x1000UL  )
#define AXI_S_Regs_BASE		( PERIPHERAL_BASE + 0x3000UL  )
#define DMAC_BASE			( PERIPHERAL_BASE + 0x4000UL  )
#define DMC_BASE			( PERIPHERAL_BASE + 0x5000UL  )
#define RTC_BASE			( PERIPHERAL_BASE + 0x7000UL  )
#define DIT_BASE			( PERIPHERAL_BASE + 0x8000UL  )
#define WDOG_BASE			( PERIPHERAL_BASE + 0x9000UL  )
#define GPIO_BASE			( PERIPHERAL_BASE + 0xA000UL  )
#define UART0_BASE			( PERIPHERAL_BASE + 0xB000UL  )
#define UART1_BASE			( PERIPHERAL_BASE + 0xC000UL  )
#define SPI_BASE			( PERIPHERAL_BASE + 0xD000UL  )
#define EXTIRC_BASE			( PERIPHERAL_BASE + 0xE000UL  )
#define VIC_BASE			( PERIPHERAL_BASE + 0x20000UL )
#define USB_BASE			( PERIPHERAL_BASE + 0x28000UL )

#define PERIPHERAL_LENGTH 	( 0x3FFFFUL    )

	/*Memory macro defs*/
#define AMB0_0_START_ADDR	( 0x0UL )
#define AMB0_0_LENGTH		( 0x3FFFFUL    )
#define AMB0_START_ADDR		( 0xC0000000UL )
#define AMB0_LENGTH			( 0x3FFFFUL    )
#define AMB1_START_ADDR		( 0xC0040000UL )
#define AMB1_LENGTH			( 0x3FFFFUL    )

#define SMB0_START_ADDR		( 0x00100000UL )
#define SMB0_LENGTH			( 0x1FFFFUL    )
#define SMB1_START_ADDR		( 0x00120000UL )
#define SMB1_LENGTH			( 0x1FFFFUL    )
#define SMB2_START_ADDR		( 0x00140000UL )
#define SMB2_LENGTH			( 0x1FFFFUL    )
#define SMB3_START_ADDR		( 0x00160000UL )
#define SMB3_LENGTH			( 0x1FFFFUL    )

#define NM1B0_START_ADDR	( 0x00200000UL )
#define NM1B1_LENGTH		( 0x1FFFFUL    )
#define NM1B2_START_ADDR	( 0x00220000UL )
#define NM1B3_LENGTH		( 0x1FFFFUL    )
#define NM2B0_START_ADDR	( 0x00240000UL )
#define NM2B1_LENGTH		( 0x1FFFFUL    )
#define NM2B2_START_ADDR	( 0x00260000UL )
#define NM2B3_LENGTH		( 0x1FFFFUL    )

	/* Defining active UART console number */
#define UART_CONSOLE UART_0



	/*Common macro defs*/
#define CONCAT(x, y) x##y

#define SET_BIT_FIELD(dest, field, bits) ((dest) = \
    ((dest) & ( ~(CONCAT(field, _MASK) << CONCAT(field, _OFFS))   ) ) | (((bits) << CONCAT(field, _OFFS))& \
     (CONCAT(field, _MASK)<< CONCAT(field, _OFFS)) ))
// for setting bitfield FIELD to desctination address ADDR with value VAL_BITS required macro:
// FIELD_MASK	(bit_mask) - mask of value length
// FIELD_OFFS	(bit_mask) - offset of value
// then use macro SET_BIT_FIELD (ADDR, FIELD, VAL_BITS);
#define GET_BIT_FIELD(src, field) (((src) & (CONCAT(field, _MASK) << CONCAT(field, _OFFS)) ) >> CONCAT(field, _OFFS))
// for reading bitfield FIELD from adress ADDR to desctination variable VAR required macro:
// FIELD_MASK	(bit_mask)
// FIELD_OFFS	(bit_mask)
// then use macro VAR = SET_BIT_FIELD (ADDR ,FIELD);


/*TODO: need testing*/
#define _printf(M, ...) printf(M "\n\r", ##__VA_ARGS__)

#define _assert(M, ...) printf("[ASSERT] (%s:%d) " M "\n\r",\
        __FILE__, __LINE__, ##__VA_ARGS__)

#define _runtime_error(M, ...) printf("[ERROR] (%s:%d) " M "\n\r",\
        __FILE__, __LINE__, ##__VA_ARGS__)

#ifdef DEBUG
#define _debug(M, ...) printf("[DEBUG] (%s:%d) " M "\n\r",\
        __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define	_debug(M, ...)
#endif



#endif
