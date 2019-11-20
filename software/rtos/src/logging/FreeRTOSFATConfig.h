/*
 * FreeRTOSFATConfig.h
 *
 *  Created on: Nov 18, 2019
 *      Author: pauly
 */

#ifndef SRC_LOGGING_FREERTOSFATCONFIG_H_
#define SRC_LOGGING_FREERTOSFATCONFIG_H_

#include <stdint.h>

#define ffconfigBYTE_ORDER                              pdFREERTOS_LITTLE_ENDIAN
#define ffconfigHAS_CWD                                 1
#define ffconfigCWD_THREAD_LOCAL_INDEX                  0
#define ffconfigLFN_SUPPORT                             1
#define ffconfigSHORTNAME_CASE                          1
#define ffconfigFAT12_SUPPORT                           1
#define ffconfigOPTIMISE_UNALIGNED_ACCESS               1
#define ffconfigCACHE_WRITE_THROUGH                     1
#define ffconfigWRITE_BOTH_FATS                         1
#define ffconfigWRITE_FREE_COUNT                        0
#define ffconfigTIME_SUPPORT                            0
#define ffconfigREMOVABLE_MEDIA                         1
#define ffconfigMOUNT_FIND_FREE                         1
#define ffconfigFSINFO_TRUSTED                          0
#define ffconfigPATH_CACHE                              0
#define ffconfigHASH_CACHE                              0
#define ffconfigMKDIR_RECURSIVE                         1
//#define ffconfigBLKDEV_USES_SEM                         1
#define ffconfigMALLOC( size )                          pvPortMalloc( size )
#define ffconfigFREE( size )                            vPortFree( size )
#define ffconfigNUM_SUPPORT                             1
#define ffconfigMAX_PARTITIONS                          2
#define ffconfigMAX_FILE_SYS                            2
#define ffconfigDRIVER_BUSY_SLEEP_MS                    20
#define ffconfigFPRINTF_SUPPORT                         0
//#define ffconfigFPRINTF_BUFFER_LENGTH                   128
#define ffconfigINLINE_MEMORY_ACCESS                    0
#define ffconfigFAT_CHECK                               1
#define ffconfigMAX_FILENAME                            20
#define ffconfigSDIO_DRIVER_USES_INTERRUPT              0
#define ffconfigUSE_DELTREE                             0



#endif /* SRC_LOGGING_FREERTOSFATCONFIG_H_ */
