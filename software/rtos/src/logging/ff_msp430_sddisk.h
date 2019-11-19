#ifndef FF_MSP430_SDDISK_H
#define FF_MSP430_SDDISK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ff_headers.h"
#include "ff_sys.h"
#include <MMC.h>

#define sdSECTOR_SIZE               512UL

#define sdPARTITION_NUMBER          0

#define sdSIGNATURE                 0x12345678


FF_Disk_t *FF_SDDiskInit(char *pcName, uint8_t *pucDataBuffer, uint32_t ulSectorCount, size_t xIOManagerCacheSize);

int32_t prvReadSD(uint8_t *pucDestination, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk);

int32_t prvWriteSD(uint8_t *pucSource, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FF_MSP430_SDDISK_H */
