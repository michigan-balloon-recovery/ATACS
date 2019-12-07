#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif





// -------------------------------------------------------------- //
// -------------------- include dependencies -------------------- //
// -------------------------------------------------------------- //

// standard libraries
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
// FreeRTOS
#include "FreeRTOS.h"
#include "semphr.h"
// FATfs
#include "ff.h"
// application drivers
#include "hal_SPI.h"
#include "sensors.h"
#include "gnss.h"
#include "rockblock.h"





// ------------------------------------------------------- //
// -------------------- public macros -------------------- //
// ------------------------------------------------------- //

#define LOG_MAX_ENTRIES                     30
#define LOG_MAX_PATH_LEN                    30
#define LOG_MAX_HEADER_LEN                  100
#define LOG_PERIOD                          1000
#define LOG_TIMEOUT                         100





// ---------------------------------------------------------- //
// -------------------- type definitions -------------------- //
// ---------------------------------------------------------- //

typedef struct {
    uint16_t num_entries;
    char current_log [LOG_MAX_PATH_LEN];
    char log_header [LOG_MAX_HEADER_LEN];
    FSIZE_t fpointer;
    SemaphoreHandle_t mutex;
    bool session_started;
} log_t;





// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/*!
 * \brief Start a logging session
 * 
 * Makes logging directory, creates seed file path, and creates the first file.
 * The file names must end in a numeric sequence with leading zeros.
 * 
 * @param log_obj logging object
 * @param dir name of the directory to put logs into
 * @param seed_name name of the first log file
 * @param header string to print at the top of each log file
 * \return result of the directory creation
 * 
 */
FRESULT log_start_session(log_t *log_obj, char *dir, char* seed_name, char* header);

/*!
 * \brief Resume a logging session
 * 
 * Opens the current logging file or creates a new one if necessary.
 * Continues to add to the end of the file.
 * Prevents other tasks from logging to the same file until paused.
 * 
 * @param log_obj logging object
 * @param file returns opened file
 * \return True if session successfully started
 * 
 */
bool log_resume_session(log_t *log_obj, FIL *file);

/*!
 * \brief Pause a logging session
 * 
 * Closes the current logging file.
 * Allows other tasks to add to the log.
 * Increments entry counter (number of times before new file creation).
 * 
 * @param log_obj logging object
 * @param file opened file to close
 * @return None
 * 
 */
FRESULT log_pause_session(log_t *log_obj, FIL *file);

/*!
 * \brief Pseudo-periodic logging task
 * 
 * Calls log_rb(), log_gnss(), log_sens(), and log_aprs().
 * Delay of LOG_PERIOD milliseconds between logs (not strictly periodic).
 * 
 * \return None
 * 
 */
void task_log();

/*!
 * \brief Logs rockBLOCK data
 * 
 * Not currently implemented (doesn't write anything to the file).
 * 
 * \return None
 * 
 */
void log_rb();

/*!
 * \brief Logs GNSS data
 * 
 * Logs lattitude, longitude, and altitude.
 * Format: hr:min, latitude(decMilliSec), longitude(decMilliSec), altitude(m).
 * \return None
 * 
 */
void log_gnss();

/*!
 * \brief Logs sensor data
 * 
 * Logs humidity (plus temperature) and pressure (plus temperature).
 * Format: pressure(mBar), temperature(degC), humidity(%), temperature(degC).
 * 
 * \return None
 * 
 */
void log_sens();

/*!
 * \brief Logs APRS data
 * 
 * Not currently implemented (doesn't write anything to the file).
 * 
 * \return None
 * 
 */
void log_aprs();

#ifdef __cplusplus
}
#endif

#endif /* LOGGING_H */
