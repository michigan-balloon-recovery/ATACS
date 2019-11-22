#include "logging.h"
#include <stdlib.h>
#include "sensors.h"
#include "gnss.h"

void init()
{
	//initialize all directories
	ff_mkdir("data");
	//create folders for each data source
	ff_mkdir("data/rb");
	ff_mkdir("data/gps");
	ff_mkdir("data/sensor");
	ff_mkdir("data/aprs");
	
	char rbLastFile [7] = "000000";
	char gpsLastFile [7] = "000000";
	char sensorLastFile [7] = "000000";
	char aprsLastFile [7] = "000000";
	
	getLastFileName("data/rb",rbLastFile);
	getLastFileName("data/gps",gpsLastFile);
	getLastFileName("data/sensor",sensorLastFile);
	getLastFileName("data/aprs",aprsLastFile);
	
	convertFileName(rbLastFileName, rbFileName);
	convertFileName(gpsLastFileName, gpsFileName);
	convertFileName(sensorLastFileName, sensorFileName);
	convertFileName(aprsLastFileName, aprsFileName);	
}

void convertFileName(char *fileNameBefore, char *fileNameAfter)
{
	int toConvert = atoi(fileNameBefore);
	
	toConvert++;
	
	ltoa(toConvert,fileNameBefore);	
	
	int length = strlen(fileNameBefore);
	//6 is number of 0's in file name
	int zeros = 6-length; 
	
	int i;
	for(i = 0; i<length; ++i)
	{
		fileNameAfter[i] = 0; 
	}
	
	int j;
	for(j = 0; j<length; ++j, ++i)
	{
		fileNameAfter[i] = fileNameBefore[j];
	}
}


void getLastFileName(char *directory, char *fileName)
{
	//figure out what file number we are on 
	FF_FindData_t *findStuct;
	findStruct = (FF_FindData_t *) pvPortMalloc(sizeof(FF_FindData_t));
	memset(findStruct, 0x00, sizeof(FF_FindData_t));
	char *temp
	
	if(ff_findfirst(directory,findStruct) == 0)
	{
		while(ff_findnext(findStruct) == 0)
		{
			temp = findStuct->pcFileName;
		}
		
		int i;
		for(i = 0; i<6; ++i)
		{
			fileName[i] = temp[i];
		}
	}
	
	vPortFree(findStruct);
}

void writeRB(char *fileName)
{
	//go to rock block directory 
	ff_chdir("data/rb");
	
	//open file for writing. If exist append data else create file
	rbSourceFile = ff_fopen(fileName,"a");
	
	if(rbSourceFile != NULL)
	{
		//write data to the file
		
		rbDataCount++;
		
		ff_fclose(rbSourceFile);
	}
}

void writeGPS(char *fileName)
{
	//go to gps directory 
	ff_chdir("data/gps");
	
	gpsSoruceFile = ff_fopen(fileName, "a");
	
	if(gpsSourceFile != NULL)
	{
		gnss_t *gpsObj; 
		
		//write gps time
		gnss_time_t *time;
		if(gnss_get_time(gpsObj,time))
		{
			char *hStr;
			char *mStr;
			ltoa(hStr,time->hour);
			ltoa(mStr,time->min);
			//writes hours:minutes
			int length = strlen(hStr);
			ff_fwrite(hStr,1,length,gpsSoruceFile);
			ff_fwrite(":",1,1,gpsSoruceFile);
			length = strlen(mStr);
			ff_fwrite(mStr,1,length,gpsSourceFile);
		}
		else
		{
			ff_fwrite("???",1,3,gpsSourceFile);
		}
		ff_fwrite(",",1,1,gpsSourceFile);
		
		//write gps location 
		gnss_coordinate_pair_t *location; 
		if(gnss_get_location(gpsObj,location))
		{
			//int32_t gnss_coord_to_decSec(gnss_coordinate_t *coordinate);
			int32_t latitude;
			int32_t longitude;
			latitude = gnss_coord_to_decSec(location->latitude);
			longitude = gnss_coord_to_decSec(location->longitude);
			
			char *lat
			char *lon
			ltoa(lat,latitude);
			ltoa(lon,longitude);
			int length = strlen(lat);
			ff_fwrite(lat,1,length,gpsSoruceFile);
			ff_fwrite(",",1,1,gpsSourceFile);
			length = strlen(lon);
			ff_fwrite(lon,1,length,gpsSourceFile);
		}
		else
		{
			ff_fwrite("???",1,3,gpsSourceFile);
		}
		ff_fwrite(",",1,1,gpsSourceFile);
		
		//write gps altitude
		int32_t *altitude;
		if(gnss_get_altitude(gnss_obj,altitude))
		{
			char *aStr;
			ltoa(aStr,altitude);
			int length = strlen(aStr);
			ff_fwrite(aStr,1,length,gpsSourceFile);
		}
		else
		{
			ff_fwrite("???",1,3,gpsSourceFile);
		}
		ff_fwrite(",",1,1,gpsSourceFile);
		
		
		gpsDataCount++;
		
		ff_fclose(gpsSoruceFile);
	}
}

void writeSensor(char *fileName)
{
	//go to sensor directory 
	ff_chdir("data/sensor");
	
	sensorSourceFile = ff_fopen(fileName, "a");
	
	if(sensorSourceFile != NULL)
	{
		//write pressure data
		int32_t *pressure; 
		if(sens_get_pres(pressure))
		{
			char *pStr;
			ltoa(pStr,pressure);
			int length = strlen(pStr);
			ff_fwrite(pStr,1,length,sensorSourceFile);
		}
		else
		{
			ff_fwrite("???",1,3,sensorSourceFile);
		}
		ff_fwrite(",",1,1,sensorSourceFile);
		
		//write pressure temperature data
		int32_t *temp; 
		if(sens_get_ptemp(temp))
		{
			char *tStr;
			ltoa(tStr,temp);
			int length = strlen(tStr);
			ff_fwrite(tStr,1,length,sensorSourceFile);
		}
		else
		{
			ff_fwrite("???",1,3,sensorSourceFile);
		}
		ff_fwrite(",",1,1,sensorSourceFile);
		
		//write humidity data
		int32_t *humidity; 
		if(sens_get_humid(humidity))
		{
			char *hStr;
			ltoa(hStr,humidity);
			int length = strlen(hStr);
			ff_fwrite(hStr,1,length,sensorSourceFile);
		}
		else
		{
			ff_fwrite("???",1,3,sensorSourceFile);
		}
		ff_fwrite(",",1,1,sensorSourceFile);
		
		//write humidity temperature data
		int32_t *temp2; 
		if(sens_get_htemp(temp2))
		{
			char *t2Str;
			ltoa(t2Str,temp2);
			int length = strlen(t2Str);
			ff_fwrite(t2Str,1,length,sensorSourceFile);
		}
		else
		{
			ff_fwrite("???",1,3,sensorSourceFile);
		}
		ff_fwrite(",",1,1,sensorSourceFile);

		sensorDataCount++;
		
		ff_fclose(sensorSourceFile);
	}
}

void writeAPRS(char *fileName)
{
	//go to aprs directory 
	ff_chdir("data/aprs");
	
	aprsSourceFile = ff_fopen(fileName, "a");
	
	if(aprsSourceFile != NULL)
	{
		//write data to file, probably when last transmitted 
		
		aprsDataCount++
		
		ff_fclose(aprsSourceFile);
	}
}

void writeToFiles()
{
	/*need to add functionality to check if files should be written to
	  these may need to be in separate functions */
	if(rbDataCount > maxData)
	{
		char tempFile = rbFileName;
		convertFileName(tempFile,rbFileName);
	}
	writeRB(rbFileName);
	
	if(gpsDataCount > maxData)
	{
		char tempFile = gpsFileName;
		convertFileName(tempFile,gpsFileName);
	}
	writeGPS(gpsFileName);
	
	if(sensorDataCount > maxData)
	{
		char tempFile = sensorFileName;
		convertFileName(tempFile,sensorFileName);
	}
	writeSensor(sensorFileName);
	
	if(aprsDataCount > maxData)
	{
		char tempFile = aprsFileName;
		convertFileName(tempFile,aprsFileName);
	}
	writeAPRS(aprsFileName);
}





































