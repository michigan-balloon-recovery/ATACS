//----------------------------------------------------------------------------
//  This include file contains definitions specific to the hardware board.
//----------------------------------------------------------------------------
// ********************************************************

//----------------------------------------------------------------------------
// SPI/UART port selections.  Select which port will be used for the interface 
//----------------------------------------------------------------------------
#define SPI_SER_INTF      SER_INTF_USCIB3  // Interface to MMC

#include <msp430.h>

// SPI port definitions
#define SPI_PxSEL         P10SEL
#define SPI_PxDIR         P10DIR
#define SPI_PxIN          P10IN
#define SPI_PxOUT         P10OUT
#define SPI_SIMO          0x02
#define SPI_SOMI          0x04
#define SPI_UCLK          0x08


// SPI port definitions
#define MMC_PxSEL         SPI_PxSEL
#define MMC_PxDIR         SPI_PxDIR
#define MMC_PxIN          SPI_PxIN
#define MMC_PxOUT         SPI_PxOUT
#define MMC_SIMO          SPI_SIMO
#define MMC_SOMI          SPI_SOMI
#define MMC_UCLK          SPI_UCLK

// Chip Select
#define MMC_CS_PxOUT      P10OUT
#define MMC_CS_PxDIR      P10DIR
#define MMC_CS            0x01

#define CS_LOW()    MMC_CS_PxOUT &= ~MMC_CS               // Card Select
#define CS_HIGH()   while(halSPITXDONE); MMC_CS_PxOUT |= MMC_CS  // Card Deselect

#define DUMMY_CHAR 0xFF
