#include "ff.h"
#include "diskio.h"
#include "MMC.h"

static DWORD sddisk_size = 0;

static DSTATUS sddisk_status = STA_NOINIT;

DSTATUS disk_status (BYTE pdrv) {
    return sddisk_status;
}

DSTATUS disk_initialize (BYTE pdrv) {
    BYTE status, timeout = 0;
    DWORD sd_size;

    do {
        status = mmcInit();
        timeout++;
        if (timeout == 150) {
            sddisk_status = STA_NOINIT;
            return sddisk_status;
            break;
        }
    } while(status != 0);

    sddisk_size = mmcReadCardSize();

    sddisk_status = 0;

    return sddisk_status;
}

DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
    WORD i;
    BYTE rval;
    for(i = 0; i < count; i++) {
        rval = mmcReadSector((sector + i), (buff + 512*i));
    }
    return (rval == MMC_SUCCESS) ? RES_OK : RES_ERROR;
}

DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
    WORD i;
    BYTE rval;
    for(i = 0; i < count; i++) {
        rval = mmcWriteSector((sector + i), (buff + 512*i));
    }
    return (rval == MMC_SUCCESS) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff) {
    DRESULT res;
    switch(cmd) {
    case CTRL_SYNC:
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }
    return res;
}

DWORD get_fattime() {
    return 1;
}

