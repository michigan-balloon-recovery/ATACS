#include "logging.h"

FATFS file_sys;

extern ROCKBLOCK_t rb;
extern gnss_t GNSS;
extern sensor_data_t sensor_data;


void log_create_new(log_t *log_obj);
void log_convert_file_name(char *fileName);

void task_log() {
    log_init();
    while(1){
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
        if(rb.is_valid) {
            log_rb();
        }
        if(GNSS.is_valid) {
//            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
            log_gnss();
//            GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
//            if(GNSS.last_fix.quality != no_fix) {
//                GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN4);
//            }
//            else {
//                GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN4);
//            }
        }
        if(sensor_data.is_valid) {
//            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN3);
            log_sens();
//            GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN3);
        }
//        log_aprs();
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void log_init()
{
    FRESULT res;
    // mount drive
//    GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
    res = f_mount(&file_sys, "", 1);
//    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    vTaskDelay(1000);
	//initialize all directories
//    GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
//	res = f_mkdir("data");
//	GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    vTaskDelay(1000);


	//create folders for each data source
//    GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
	res = f_mkdir("rb");
//	GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    vTaskDelay(1000);

//    GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
	res = f_mkdir("gnss");
//	GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    vTaskDelay(1000);

//    GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
	res = f_mkdir("sens");
//	GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    vTaskDelay(1000);

//    GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
	res = f_mkdir("aprs");
//	GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    vTaskDelay(1000);



	rb_log.num_entries = maxFileNameLength + 1;
	gnss_log.num_entries = maxFileNameLength + 1;
    sens_log.num_entries = maxFileNameLength + 1;
	aprs_log.num_entries = maxFileNameLength + 1;

	strcpy(rb_log.current_log, "rb/000000.txt");
    strcpy(gnss_log.current_log, "gnss/000000.txt");
    strcpy(sens_log.current_log, "sens/000000.txt");
    strcpy(aprs_log.current_log, "aprs/000000.txt");
}

void log_rb()
{
	
	//open file for writing. If exist append data else create file
    FRESULT res;

    if(rb_log.num_entries > maxFileNameLength){
        log_create_new(&rb_log);
    }

    FIL file;
    UINT bw;
    res = f_open(&file, rb_log.current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    res = f_lseek(&file, rb_log.fpointer);

    if(res == FR_OK)
    {
		//write data to the file

        rb_log.num_entries++;
		
        rb_log.fpointer = file.fptr;
		f_close(&file);
	}
}

void log_gnss()
{

    FRESULT res;

    if(gnss_log.num_entries > maxFileNameLength){
        log_create_new(&gnss_log);
    }

    FIL file;
    UINT bw;
//    taskENTER_CRITICAL();
    res = f_open(&file, gnss_log.current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    res = f_lseek(&file, gnss_log.fpointer);

    if(res == FR_OK)
    {
		//write gps time
		gnss_time_t time;
		if(gnss_get_time(&GNSS,&time))
		{
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
		else
		{
			f_write(&file,"???",3,&bw);
		}
		f_write(&file,",",1,&bw);
		
		//write gps location 
		gnss_coordinate_pair_t location;
		if(gnss_get_location(&GNSS,&location))
		{
			//int32_t gnss_coord_to_decSec(gnss_coordinate_t *coordinate);
			int32_t latitude;
			int32_t longitude;
			latitude = gnss_coord_to_decSec(&location.latitude);
			longitude = gnss_coord_to_decSec(&location.longitude);
			
			char lat[10];
			char lon[10];
			ltoa(latitude,lat);
			ltoa(longitude,lon);
			int length = strlen(lat);
			f_write(&file,lat,length,&bw);
			f_write(&file,",",1,&bw);
			length = strlen(lon);
			f_write(&file,lon,length,&bw);
		}
		else
		{
            f_write(&file,"???",3,&bw);
		}
        f_write(&file,",",1,&bw);
		
		//write gps altitude
		int32_t altitude;
		if(gnss_get_altitude(&GNSS,&altitude))
		{
			char aStr[20];
			ltoa(altitude,aStr);
			int length = strlen(aStr);
			f_write(&file,aStr,length,&bw);
		}
		else
		{
            f_write(&file,"???",3,&bw);
		}
        f_write(&file,"\n",1,&bw);
		
        gnss_log.num_entries++;
		
        gnss_log.fpointer = file.fptr;
		f_close(&file);
//		taskEXIT_CRITICAL();
	}
}

void log_sens()
{

    FRESULT res;

    if(sens_log.num_entries > maxFileNameLength){
        log_create_new(&sens_log);
    }

    FIL file;
    UINT bw;
//    taskENTER_CRITICAL();
    res = f_open(&file, sens_log.current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    res = f_lseek(&file, sens_log.fpointer);

	if(res == FR_OK)
	{
		//write pressure data
		int32_t pressure;
		if(sens_get_pres(&pressure))
		{
			char pStr[20];
			ltoa(pressure,pStr);
			int length = strlen(pStr);
			f_write(&file,pStr,length,&bw);
		}
		else
		{
			f_write(&file,"???",3,&bw);
		}
		f_write(&file,",",1,&bw);
		
		//write pressure temperature data
		int32_t temp;
		if(sens_get_ptemp(&temp))
		{
			char tStr[20];
			ltoa(temp,tStr);
			int length = strlen(tStr);
			f_write(&file,tStr,length,&bw);
		}
		else
		{
            f_write(&file,"???",3,&bw);
		}
        f_write(&file,",",1,&bw);
		
		//write humidity data
		int32_t humidity;
		if(sens_get_humid(&humidity))
		{
			char hStr[20];
			ltoa(humidity,hStr);
			int length = strlen(hStr);
			f_write(&file,hStr,length,&bw);
		}
		else
		{
            f_write(&file,"???",3,&bw);
		}
        f_write(&file,",",1,&bw);
		
		//write humidity temperature data
		int32_t temp2;
		if(sens_get_htemp(&temp2))
		{
			char t2Str[20];
			ltoa(temp2,t2Str);
			int length = strlen(t2Str);
			f_write(&file,t2Str,length,&bw);
		}
		else
		{
            f_write(&file,"???",3,&bw);
		}
        f_write(&file,"\n",1,&bw);

		sens_log.num_entries++;
		
        sens_log.fpointer = file.fptr;
		f_close(&file);
//		taskEXIT_CRITICAL();
	}
}

void log_aprs()
{

    FRESULT res;

    if(aprs_log.num_entries > maxFileNameLength){
        log_create_new(&aprs_log);
    }

    FIL file;
    UINT bw;
//    taskENTER_CRITICAL();
    res = f_open(&file, aprs_log.current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    res = f_lseek(&file, aprs_log.fpointer);
    if(res == FR_OK)
    {
		//write data to file, probably when last transmitted 

        aprs_log.num_entries++;
		
        aprs_log.fpointer = file.fptr;
		f_close(&file);
//		taskEXIT_CRITICAL();
	}
}

void log_convert_file_name(char *fileName)
{
    char* end = fileName;
    while(*end != '.'){
        end++;
    }
    end--;

    while(*end == '9'){
        *end = '0';
        end--;
    }
    *end += 1;
}

void log_create_new(log_t *log_obj)
{
    taskENTER_CRITICAL();
    log_obj->fpointer = 0;
    while(f_stat(log_obj->current_log, NULL) == FR_OK){
        log_convert_file_name(log_obj->current_log);
    }
    log_obj->num_entries = 0;
    taskEXIT_CRITICAL();
}
