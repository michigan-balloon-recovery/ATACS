#include "logging.h"


FATFS file_sys;

extern ROCKBLOCK_t rb;
extern gnss_t GNSS;
extern sensor_data_t sensor_data;


void log_create_new(char* initial_log);
void log_convert_file_name(char *fileName);

void task_log() {
    log_init();
    while(1){
        if(rb.is_valid) {
            log_rb();
        }
        if(GNSS.is_valid) {
            log_gnss();
        }
        if(sensor_data.is_valid) {
            log_sens();
        }
//        log_aprs();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

void log_init()
{
    FRESULT res;
    // mount drive
    res = f_mount(&file_sys, "", 1);
	//initialize all directories
	res = f_mkdir("data");
	//create folders for each data source
	res = f_mkdir("data/rb");
	res = f_mkdir("data/gnss");
	res = f_mkdir("data/sens");
	res = f_mkdir("data/aprs");

	rb_log.num_entries = maxFileNameLength + 1;
	gnss_log.num_entries = maxFileNameLength + 1;
    sens_log.num_entries = maxFileNameLength + 1;
	aprs_log.num_entries = maxFileNameLength + 1;

	strcpy(rb_log.current_log, "data/rb/000000.txt");
    strcpy(gnss_log.current_log, "data/gnss/000000.txt");
    strcpy(sens_log.current_log, "data/sens/000000.txt");
    strcpy(aprs_log.current_log, "data/aprs/000000.txt");
}

void log_rb()
{
	
	//open file for writing. If exist append data else create file
    FRESULT res;

    if(rb_log.num_entries > maxFileNameLength){
        log_create_new(rb_log.current_log);
    }

    FIL file;
    UINT* bw;
    res = f_open(&file, rb_log.current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

    if(res == FR_OK)
    {
		//write data to the file

        rb_log.num_entries++;
		
		f_close(&file);
	}
}

void log_gnss()
{

    FRESULT res;

    if(gnss_log.num_entries > maxFileNameLength){
        log_create_new(gnss_log.current_log);
    }

    FIL file;
    UINT* bw;
    res = f_open(&file, gnss_log.current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

    if(res == FR_OK)
    {
		//write gps time
		gnss_time_t *time;
		if(gnss_get_time(&GNSS,time))
		{
			char *hStr;
			char *mStr;
			ltoa(hStr,time->hour);
			ltoa(mStr,time->min);
			//writes hours:minutes
			int length = strlen(hStr);
			f_write(&file,hStr,length,bw);
			f_write(&file,":",1,bw);
			length = strlen(mStr);
			f_write(&file,mStr,length,bw);
		}
		else
		{
			f_write(&file,"???",3,bw);
		}
		f_write(&file,",",1,bw);
		
		//write gps location 
		gnss_coordinate_pair_t *location; 
		if(gnss_get_location(&GNSS,location))
		{
			//int32_t gnss_coord_to_decSec(gnss_coordinate_t *coordinate);
			int32_t latitude;
			int32_t longitude;
			latitude = gnss_coord_to_decSec(&location->latitude);
			longitude = gnss_coord_to_decSec(&location->longitude);
			
			char *lat;
			char *lon;
			ltoa(lat,latitude);
			ltoa(lon,longitude);
			int length = strlen(lat);
			f_write(&file,lat,length,bw);
			f_write(&file,",",1,bw);
			length = strlen(lon);
			f_write(&file,lon,length,bw);
		}
		else
		{
            f_write(&file,"???",3,bw);
		}
        f_write(&file,",",1,bw);
		
		//write gps altitude
		int32_t *altitude;
		if(gnss_get_altitude(&GNSS,altitude))
		{
			char *aStr;
			ltoa(aStr,altitude);
			int length = strlen(aStr);
			f_write(&file,aStr,length,bw);
		}
		else
		{
            f_write(&file,"???",3,bw);
		}
        f_write(&file,"\n",1,bw);
		
        gnss_log.num_entries++;
		
		f_close(&file);
	}
}

void log_sens()
{

    FRESULT res;

    if(sens_log.num_entries > maxFileNameLength){
        log_create_new(sens_log.current_log);
    }

    FIL file;
    UINT* bw;
    res = f_open(&file, sens_log.current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

	if(res == FR_OK)
	{
		//write pressure data
		int32_t *pressure; 
		if(sens_get_pres(pressure))
		{
			char *pStr;
			ltoa(pStr,pressure);
			int length = strlen(pStr);
			f_write(&file,pStr,length,bw);
		}
		else
		{
			f_write(&file,"???",3,bw);
		}
		f_write(&file,",",1,bw);
		
		//write pressure temperature data
		int32_t *temp; 
		if(sens_get_ptemp(temp))
		{
			char *tStr;
			ltoa(tStr,temp);
			int length = strlen(tStr);
			f_write(&file,tStr,length,bw);
		}
		else
		{
            f_write(&file,"???",3,bw);
		}
        f_write(&file,",",1,bw);
		
		//write humidity data
		int32_t *humidity; 
		if(sens_get_humid(humidity))
		{
			char *hStr;
			ltoa(hStr,humidity);
			int length = strlen(hStr);
			f_write(&file,hStr,length,bw);
		}
		else
		{
            f_write(&file,"???",3,bw);
		}
        f_write(&file,",",1,bw);
		
		//write humidity temperature data
		int32_t *temp2; 
		if(sens_get_htemp(temp2))
		{
			char *t2Str;
			ltoa(t2Str,temp2);
			int length = strlen(t2Str);
			f_write(&file,t2Str,length,bw);
		}
		else
		{
            f_write(&file,"???",3,bw);
		}
        f_write(&file,"\n",1,bw);

		sens_log.num_entries++;
		
		f_close(&file);
	}
}

void log_aprs()
{

    FRESULT res;

    if(aprs_log.num_entries > maxFileNameLength){
        log_create_new(aprs_log.current_log);
    }

    FIL file;
    UINT* bw;
    res = f_open(&file, aprs_log.current_log, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

    if(res == FR_OK)
    {
		//write data to file, probably when last transmitted 

        aprs_log.num_entries++;
		
		f_close(&file);
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

void log_create_new(char* initial_log)
{
    while(f_stat(initial_log, NULL) == FR_OK){
        log_convert_file_name(initial_log);
    }
}
