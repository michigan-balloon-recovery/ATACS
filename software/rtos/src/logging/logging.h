#include <stdint.h>
#include "ff_headers.h"

//max data points per file
#define maxData 1000

//variables to keep track of how much data has been written to the files
uint32_t rbDataCount = 0;
uint32_t gpsDataCount = 0;
uint32_t sensorDataCount = 0;
uint32_t aprsDataCount = 0;

//file name for data
char rbFileName [11] = "000000.txt";
char gpsFileName [11] = "000000.txt";
char sensorFileName [11] = "000000.txt";
char aprsFileName [11] = "000000.txt"; 



void log_init();

void log_get_last_filename(char *directory, char *fileName);

void log_RB(char *fileName);

void log_GNSS(char *fileName);

void log_sensor(char *fileName);

void log_APRS(char *fileName);

void log_write_to_files();

void log_convert_file_name(char *fileNameBefore, char *fileNameAfter);
