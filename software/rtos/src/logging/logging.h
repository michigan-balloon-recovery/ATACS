
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



void init();

void getLastFileName(char *directory, char *fileName);

void writeRB(char *fileName);

void writeGPS(char *fileName);

void writeSensor(char *fileName);

void writeAPRS(char *fileName);

void writeToFiles();

void convertFileName(char *fileNameBefore, char *fileNameAfter);
