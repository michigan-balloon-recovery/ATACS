#include "ff_msp430_sddisk.h"

FF_Disk_t *FF_SDDiskInit(char *pcName, uint8_t *pucDataBuffer, uint32_t ulSectorCount, size_t xIOManagerCacheSize) {
    FF_Error_t xError;
    FF_Disk_t *pxDisk = NULL;
    FF_CreationParameters_t xParameters;

    // check that the cache size is valid
    configASSERT( (xIOManagerCacheSize % sdSECTOR_SIZE) == 0 );
    configASSERT(xIOManagerCacheSize >= (2*sdSECTOR_SIZE));

    // allocate the FF_Disk_t object
    pxDisk = pvPortMalloc(sizeof(FF_Disk_t));

    if(pxDisk != NULL) {
        // initialize the FF_Disk_t object to 0
        memset(pxDisk, 0,sizeof(FF_Disk_t));

        // sd card specific parameters stored in pxDisk->pvTag

        // sd card signature set in pxDisk->ulSignature
        pxDisk->ulSignature = sdSIGNATURE;

        // store number of sectors for bounds checking
        pxDisk->ulNumberOfSectors = ulSectorCount;

        // initialize parameters struct
        memset(&xParameters, 0, sizeof(FF_CreationParameters_t));
        xParameters.pucCacheMemory = NULL;
        xParameters.ulMemorySize = xIOManagerCacheSize;
        xParameters.ulSectorSize = sdSECTOR_SIZE;
        xParameters.fnWriteBlocks = prvWriteSD;
        xParameters.fnReadBlocks = prvReadSD;
        xParameters.pxDisk = pxDisk;

        // device is set as non-reentrant
        xParameters.pvSemaphore = xSemaphoreCreateRecursiveMutex();
        xParameters.xBlockDeviceIsReentrant = pdFALSE;

        // create IO manager
        pxDisk->pxIOManager = FF_CreateIOManger(&xParameters, &xError);

        if( (pxDisk->pxIOManager != NULL) && (FF_isERR(xError) == pdFALSE) ) {
            pxDisk->xStatus.bIsInitialised = pdTRUE;
            pxDisk->xStatus.bPartitionNumber = sdPARTITION_NUMBER;

            // mount partition
            xError = FF_Mount(pxDisk, sdPARTITION_NUMBER);
            pxDisk->xStatus.bIsMounted = pdTRUE;

            if(FF_isERR(xError) == pdFALSE) {
                // add to virtual file system
                FF_FS_Add(pcName, pxDisk->pxIOManager);
            }
        }
        else {
            // clean up disk memory since the disk IO manager couldn't be allocated
//            FF_SDDiskDelete(pxDisk);
            pxDisk = NULL;
        }
    }
    return pxDisk;
}

int32_t prvWriteSD(uint8_t *pucSource, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk) {
    uint8_t i;
    for(i = 0; i < ulSectorCount; i++) {
        mmcWriteSector(ulSectorNumber, pucSource);
    }

    return FF_ERR_NONE;
}

int32_t prvReadSD(uint8_t *pucDestination, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk) {
    uint8_t i;
    for(i = 0; i < ulSectorCount; i++) {
        mmcReadSector(ulSectorNumber, pucDestination);
    }

    return FF_ERR_NONE;
}
