#include "logging.h"
/*-------------------------------------------------------------------------------- /
/ ATACS logging driver
/ -------------------------------------------------------------------------------- /
/ Part of the ATACS (Aerial Termination And Communication System) project
/       https://github.com/michigan-balloon-recovery/ATACS
/       released under the GPLv2 license (see ATACS/LICENSE in git repository)
/ Creation Date: November 2019
/ Contributors: Kirsten Apel, Paul Young
/ --------------------------------------------------------------------------------*/




// ---------------------------------------------------------- //
// -------------------- global variables -------------------- //
// ---------------------------------------------------------- //
FATFS file_sys;
log_t rb_log = {.session_started = false};
log_t gnss_log = {.session_started = false};
log_t aprs_log = {.session_started = false};
log_t sens_log = {.session_started = false};

extern ROCKBLOCK_t rb;
extern gnss_t GNSS;
extern sensor_data_t sensor_data;





// ------------------------------------------------------------ //
// -------------------- private prototypes -------------------- //
// ------------------------------------------------------------ //

/*!
 * \brief Create a new logging file for the specified logging object
 *
 * Takes the logging object's filename seed and increments it until a file that doesn't exist is created.
 * The new file is created and the logging object's header is written to the file.
 * 
 * @param log_obj logging object
 * @return None
 *
 */
static void log_create_new(log_t *log_obj);

/*!
 * \brief Updates the file name to the next valid name
 *
 * Increments the file name assuming the name is the ASCII representation of a decimal number.
 * Assumes the file name is long enough and overwrites characters to the left on overflow.
 * 
 * @param file_name file name to increment
 * @return None
 *
 */
static void log_convert_file_name(char *file_name);

/*!
 * \brief Initialization of periodic logging task
 * 
 * 
 * \return None
 * 
 */
static void log_init();





// -------------------------------------------------------------- //
// -------------------- FreeRTOS task & init -------------------- //
// -------------------------------------------------------------- //

void task_log() {
    log_init();
    for( ;; ) {
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
        if(GNSS.is_valid) {
            log_gnss();
        }
        if(sensor_data.humid_init && sensor_data.pres_init) {
            log_sens();
        }
        if(rb.is_valid) {
            log_rb();
        }
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
        vTaskDelay(LOG_PERIOD / portTICK_RATE_MS);
    }
}

static void log_init() {
    FRESULT res;

    // mount drive
    res = f_mount(&file_sys, "", 1);

    if(res == FR_OK) {
        // start logging sessions
        log_start_session(&gnss_log,
                            "gnss",
                            "000000.csv",
                            "GNSS log file\nhr:min,latitude(decMilliSec),longitude(decMilliSec),altitude(m)\n"
                            );
        log_start_session(&sens_log,
                            "sens",
                            "000000.csv",
                            "sensor log file\npressure(mBar),temperature(degC),humidity(%),temperature(degC)\n"
                            );
        log_start_session(&rb_log,
                            "rb",
                            "000000.csv",
                            "rockBLOCK log file\n"
                            );
        log_start_session(&aprs_log,
                            "aprs",
                            "000000.csv",
                            "APRS log file\n"
                            );
    }
}





// ---------------------------------------------------- //
// -------------------- public API -------------------- //
// ---------------------------------------------------- //

void log_gnss() {
    FIL file;
    UINT bw;

    if(log_resume_session(&gnss_log, &file)) {
        //write gps time
        gnss_time_t time;
        if(gnss_get_time(&GNSS,&time)) {
            char hStr[20];
            char mStr[20];
            ltoa(time.hour,hStr);
            ltoa(time.min,mStr);
            //writes hours:minutes
            int length = strlen(hStr);
            f_write(&file,hStr,length,&bw);
            f_write(&file,":",1,&bw);
            length = strlen(mStr);
            f_write(&file,mStr,length,&bw);
        }
        else {
            f_write(&file,"??:??",5,&bw);
        }
        f_write(&file,",",1,&bw);
        
        //write gps location 
        gnss_coordinate_pair_t location;
        if(gnss_get_location(&GNSS,&location)) {
            char lat[10];
            char lon[10];
            ltoa(location.latitude.decMilliSec,lat);
            ltoa(location.longitude.decMilliSec,lon);
            int length = strlen(lat);
            f_write(&file,lat,length,&bw);
            f_write(&file,&location.latitude.dir,1,&bw);
            f_write(&file,",",1,&bw);
            length = strlen(lon);
            f_write(&file,lon,length,&bw);
            f_write(&file,&location.longitude.dir,1,&bw);
        }
        else {
            f_write(&file,"??,??",5,&bw);
        }
        f_write(&file,",",1,&bw);
        
        //write gps altitude
        int32_t altitude;
        if(gnss_get_altitude(&GNSS,&altitude)) {
            char aStr[20];
            ltoa(altitude,aStr);
            int length = strlen(aStr);
            f_write(&file,aStr,length,&bw);
        }
        else {
            f_write(&file,"???",3,&bw);
        }
        f_write(&file,"\n",1,&bw);
        
        log_pause_session(&rb_log, &file);
    }
}

void log_sens() {
    FIL file;
    UINT bw;

    if(log_resume_session(&sens_log, &file)) {
        //write pressure data
        int32_t pressure;
        if(sens_get_pres(&pressure)) {
            char pStr[20];
            ltoa(pressure,pStr);
            int length = strlen(pStr);
            f_write(&file,pStr,length,&bw);
        }
        else {
            f_write(&file,"???",3,&bw);
        }
        f_write(&file,",",1,&bw);
        
        //write pressure temperature data
        int32_t temp;
        if(sens_get_ptemp(&temp)) {
            char tStr[20];
            ltoa(temp,tStr);
            int length = strlen(tStr);
            f_write(&file,tStr,length,&bw);
        }
        else {
            f_write(&file,"???",3,&bw);
        }
        f_write(&file,",",1,&bw);
        
        //write humidity data
        int32_t humidity;
        if(sens_get_humid(&humidity)) {
            char hStr[20];
            ltoa(humidity,hStr);
            int length = strlen(hStr);
            f_write(&file,hStr,length,&bw);
        }
        else {
            f_write(&file,"???",3,&bw);
        }
        f_write(&file,",",1,&bw);
        
        //write humidity temperature data
        int32_t temp2;
        if(sens_get_htemp(&temp2)) {
            char t2Str[20];
            ltoa(temp2,t2Str);
            int length = strlen(t2Str);
            f_write(&file,t2Str,length,&bw);
        }
        else {
            f_write(&file,"???",3,&bw);
        }
        f_write(&file,"\n",1,&bw);

        log_pause_session(&rb_log, &file);
    }
}

void log_rb() {
    FIL file;
    UINT bw;

    if(log_resume_session(&rb_log, &file)) {
        // write data to file
        // TODO: rockBLOCK logging
        log_pause_session(&rb_log, &file);
    }
}

void log_aprs() {
    FIL file;
    UINT bw;

    if(log_resume_session(&aprs_log, &file)) {
        //write data to file
        // TODO: APRS logging
        log_pause_session(&rb_log, &file);
    }
}

FRESULT log_start_session(log_t *log_obj, char *dir, char* seed_name, char* header) {
    FRESULT res;

    res = f_mkdir(dir); // make logging directory

    // fill logging object
    strcpy(log_obj->current_log, dir);
    strcat(log_obj->current_log, "/");
    strcat(log_obj->current_log, seed_name);
    strcpy(log_obj->log_header, header);
    log_obj->num_entries = 0;
    log_obj->mutex = xSemaphoreCreateMutex();

    // find first unused, valid file name
    log_create_new(log_obj);

    log_obj->session_started = true;
    return res;
}

bool log_resume_session(log_t *log_obj, FIL *file) {
    FRESULT res[2];

    if( log_obj->session_started && (xSemaphoreTake(log_obj->mutex, LOG_TIMEOUT / portTICK_RATE_MS) == pdTRUE) ) {
        // create new log file if necessary
        if(log_obj->num_entries >= LOG_MAX_ENTRIES) {
            log_create_new(log_obj);
        }

        // open file
        res[0] = f_open(file, log_obj->current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

        // move file pointer to previous location
        res[1] = f_lseek(file, log_obj->fpointer);
        if(res[1] != FR_OK) {
            f_close(file); // close file if error occurred
        }
    }
    return (res[0] == FR_OK) && (res[1] == FR_OK);
}

FRESULT log_pause_session(log_t *log_obj, FIL *file) {
    FRESULT res;
    log_obj->num_entries++; // increment entry counter
    log_obj->fpointer = file->fptr; // store file pointer location
    res = f_close(file);
    xSemaphoreGive(log_obj->mutex);
    return res;
}





// ----------------------------------------------------- //
// -------------------- private API -------------------- //
// ----------------------------------------------------- //

static void log_create_new(log_t *log_obj) {
    FIL file;
    UINT bw;
    FRESULT res;

    // find first unused file name
    while(f_stat(log_obj->current_log, NULL) == FR_OK) {
        log_convert_file_name(log_obj->current_log);
    }
    // reset entry counter
    log_obj->num_entries = 0;

    // open file and write the header
    res = f_open(&file, log_obj->current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    if(res == FR_OK) {
        f_write(&file,log_obj->log_header,strlen(log_obj->log_header),&bw);
        log_obj->fpointer = file.fptr;
        f_close(&file);
    }
}

static void log_convert_file_name(char *file_name) {
    char* ptr = file_name;

    // move pointer to end of the filename (ignore extension)
    while(*ptr != '.') {
        ptr++;
    }
    ptr--;

    // ASCII addition
    while(*ptr == '9') { // check for overflow
        *ptr = '0';
        ptr--;
    }
    *ptr += 1; // increment
}
