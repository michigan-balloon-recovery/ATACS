#include "ff_msp430_sddisk.h"

FF_Disk_t *FF_SDDiskInit(char *pcName, uint32_t ulSectorCount, size_t xIOManagerCacheSize) {
    FF_Error_t xError;
    FF_Disk_t *pxDisk = NULL;
    FF_CreationParameters_t xParameters;
    FF_PartitionParameters_t xPartition;
    BaseType_t valid;

    // check that the cache size is valid
    configASSERT( (xIOManagerCacheSize % sdSECTOR_SIZE) == 0 );
    configASSERT(xIOManagerCacheSize >= (2*sdSECTOR_SIZE));

    // allocate the FF_Disk_t object
    pxDisk = pvPortMalloc(sizeof(FF_Disk_t));

    if(pxDisk != NULL) {
        // initialize the FF_Disk_t object to 0
        memset(pxDisk, 0,sizeof(FF_Disk_t));

//        memset(pucDataBuffer, 0, ulSectorCount * sdSECTOR_SIZE);

        // sd card specific parameters stored in pxDisk->pvTag

        // sd card signature set in pxDisk->ulSignature
        pxDisk->ulSignature = sdSIGNATURE;

        // store number of sectors for bounds checking
        pxDisk->ulNumberOfSectors = ulSectorCount;

        // initialize parameters struct
        memset(&xParameters, 0, sizeof(xParameters));
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

            memset(&xPartition, 0x00, sizeof(xPartition));
            xPartition.ulSectorCount = pxDisk->ulNumberOfSectors;
            xPartition.ulHiddenSectors = 8;
            xPartition.ulInterSpace = 0;
            xPartition.xPrimaryCount = 1;
            xPartition.eSizeType = eSizeIsQuota;

//            xError = FF_Partition(pxDisk, &xPartition);

//            xError = FF_Format(pxDisk, 0, pdTRUE, pdTRUE);

            // mount partition
            do {
                xError = FF_Mount(pxDisk, sdPARTITION_NUMBER);
                pxDisk->xStatus.bIsMounted = pdTRUE;
            } while(FF_isERR(xError) != pdFALSE);

            if(FF_isERR(xError) == pdFALSE) {
                // add to virtual file system
                valid = FF_FS_Add(pcName, pxDisk);
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
