//----------------------------------------------------------------------------
//  This include file contains definitions specific to the hardware board.
//----------------------------------------------------------------------------
// ********************************************************

#include <msp430.h>
                                      // Adjust this according to the
                                      // MSP430 device being used.
// SPI port definitions               // Adjust the values for the chosen
#define SPI_PxSEL         P10SEL      // interfaces, according to the pin
#define SPI_PxDIR         P10DIR      // assignments indicated in the
#define SPI_PxIN          P10IN       // chosen MSP430 device datasheet.
#define SPI_PxOUT         P10OUT
#define SPI_SIMO          0x02
#define SPI_SOMI          0x04
#define SPI_UCLK          0x08

//----------------------------------------------------------------------------
// SPI/UART port selections.  Select which port will be used for the interface 
//----------------------------------------------------------------------------
#define SPI_SER_INTF      SER_INTF_USCIB3  // Interface to MMC




// SPI port definitions                  // Adjust the values for the chosen
#define MMC_PxSEL         SPI_PxSEL      // interfaces, according to the pin
#define MMC_PxDIR         SPI_PxDIR      // assignments indicated in the
#define MMC_PxIN          SPI_PxIN       // chosen MSP430 device datasheet.
#define MMC_PxOUT         SPI_PxOUT      
#define MMC_SIMO          SPI_SIMO
#define MMC_SOMI          SPI_SOMI
#define MMC_UCLK          SPI_UCLK

// Chip Select
#define MMC_CS_PxOUT      P10OUT
#define MMC_CS_PxDIR      P10DIR
#define MMC_CS            0x01

// Card Detect
//#define MMC_CD_PxIN       P5IN
//#define MMC_CD_PxDIR      P5DIR
//#define MMC_CD            0x40

#define CS_LOW()    MMC_CS_PxOUT &= ~MMC_CS               // Card Select
#define CS_HIGH()   while(halSPITXDONE); MMC_CS_PxOUT |= MMC_CS  // Card Deselect

#define DUMMY_CHAR 0xFF
