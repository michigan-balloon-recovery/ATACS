#include "sensors.h"
#include <driverlib.h>

/**
 * sensors.c
 */

sensor_data_t sensor_data = {.humid_init = false, .pres_init = false};

void task_pressure(void) {
//    const portTickType xFrequency = 1000 / portTICK_RATE_MS;

    sensor_data.pressureSemaphore = xSemaphoreCreateMutex();
    sens_init_pres();

    int32_t data[2] = {0, 0};
    bool pres_valid = false;

    while(1) {

        if(sens_calc_pres(data))
            pres_valid = true;
        else
            pres_valid = false;

        if(xSemaphoreTake(sensor_data.pressureSemaphore,100/portTICK_RATE_MS) == pdTRUE) {
            sensor_data.pressure = data[0];
            sensor_data.pTemp = data[1];
            sensor_data.pres_valid = pres_valid;
            xSemaphoreGive(sensor_data.pressureSemaphore);
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void task_humidity(void) {
    const portTickType xFrequency = 1000 / portTICK_RATE_MS;

    sensor_data.humiditySemaphore = xSemaphoreCreateMutex();
    sensor_data.humid_init = true;
    int32_t data[2] = {0, 0};
    bool humid_valid = false;

    while(1) {
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN3);

        if(sens_calc_humid(data))
            humid_valid = true;
        else
            humid_valid = false;

        if(xSemaphoreTake(sensor_data.humiditySemaphore, 100/portTICK_RATE_MS) == pdTRUE) {
            sensor_data.humidity = data[0];
            sensor_data.hTemp = data[1];
            sensor_data.humid_valid = humid_valid;
            xSemaphoreGive(sensor_data.humiditySemaphore);
        }

        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN3);
        vTaskDelay(xFrequency);
    }
}
 
void sens_init_pres(void) {
	uint8_t cmd[1];
	
    unsigned int j;
    volatile unsigned int i;
    for(i = 0; i < 8; i++)
        c[i] = 0;

	cmd[0] = 0x1E;
    while(i2c_write(0x77, cmd, 1) == false);
    for(j = 0; j<8; j++) {
        uint8_t data[2];
        vTaskDelay(100/portTICK_PERIOD_MS);
        cmd[0] = 0xA0 + j*2;
        while(i2c_write(0x77, cmd, 1) == false);
        vTaskDelay(100/portTICK_PERIOD_MS);
        while(i2c_read(0x77, data, 2) == false);
        c[j] = data[0];
        c[j] = c[j] << 8;
        c[j] += data[1];
    }
    sensor_data.pres_init = true;
}

void sens_enable_interrupts(void) {

    i2c_enable_interrupts();
    return;

}

bool sens_disable_interrupts(void) {

    return i2c_disable_interrupts();

}
 
bool sens_calc_pres(int32_t* return_data) {
    uint8_t data[4] = {0x0, 0x0, 0x0, 0x0};
    uint8_t cmd[1];

    int64_t pressure = 0;

    uint32_t d1, d2;

    int64_t dT, pTemp;
    int64_t offset, sens;

    //retrieves digital pressure value
    vTaskDelay(100/portTICK_PERIOD_MS);
    cmd[0] = 0x48;

    if(i2c_write(0x77, cmd, 1) == false)
        return false;

    vTaskDelay(100/portTICK_PERIOD_MS);
    cmd[0] = 0x00;

    if(i2c_write(0x77, cmd, 1) == false)
        return false;


    vTaskDelay(100/portTICK_PERIOD_MS);

    if(i2c_read(0x77, data, 3) == false)
        return false;


    d1 = data[0];
    d1 = d1 << 8;
    d1 += data[1];
    d1 = d1 << 8;
    d1 += data[2];

    //retrieves digital temperature value
    cmd[0] = 0x58;

    if(i2c_write(0x77, cmd, 1) == false)
        return false;

    vTaskDelay(100/portTICK_PERIOD_MS);
    cmd[0] = 0x00;
    if(i2c_write(0x77, cmd, 1) == false)
        return false;

    vTaskDelay(100/portTICK_PERIOD_MS);

    if(i2c_read(0x77, data, 3) == false)
        return false;

    d2 = data[0];
    d2 = d2 << 8;
    d2 += data[1];
    d2 = d2 << 8;
    d2 += data[2];

    //calculate temperature
    dT = c[5];
    dT = dT << 8;
    dT = d2 - dT;

    pTemp = c[6];
    pTemp *= dT;
    pTemp = pTemp >> 23;
    pTemp += 2000;

    //calculate temperature compensated pressure
    int64_t temporary;
    offset = c[2];
    offset = offset << 16;
    temporary = c[4];
    temporary *= dT;
    temporary = temporary >> 7;
    offset = offset + temporary;

    sens = c[1];
    sens = sens << 15;
    temporary = c[3];
    temporary *= dT;
    temporary = temporary >> 8;
    sens = sens + temporary;

    pressure = d1;
    pressure *= sens;
    pressure = pressure >> 21;
    pressure -= offset;
    pressure = pressure >> 15; // 32768
    pressure /= 100;

    pTemp += 50;
    pTemp /= 100;

    return_data[0] = pressure;
    return_data[1] = pTemp;
    return true;
}

bool sens_calc_humid(int32_t *return_data) {
    uint32_t hum = 0;
    uint8_t data[4] = {0x0, 0x0, 0x0, 0x0};
    int32_t temp = 0;

    if(i2c_write(0x27, 0, 0) == false)
        return false;

    vTaskDelay(50/portTICK_PERIOD_MS);

    if(i2c_read(0x27, data, 4) == false)
        return false;

    hum = data[0] & 0x3F;
    hum = hum << 8;
    hum += data[1];
    hum = 100*hum;
    //hum += 8191; // add 1/2 LSB for accurate rounding
    hum = hum/16382;
    temp = data[2];
    temp = temp << 6;
    temp += (data[3] >> 2);
    temp = 165*temp;
    //temp += 8191; // add 1/2 LSB for accurate rounding
    temp = temp / 16382;
    temp -= 40;

    return_data[0] = hum;
    return_data[1] = temp;
    return true;
}

bool sens_get_pres(int32_t* pressure) {
	if(!sensor_data.pres_init || xSemaphoreTake(sensor_data.pressureSemaphore,100) == pdFALSE)
		return false;

	*pressure = sensor_data.pressure;
	bool toReturn = sensor_data.pres_valid;
    xSemaphoreGive(sensor_data.pressureSemaphore);
    return toReturn;
}

bool sens_get_ptemp(int32_t* temp) {
	if(!sensor_data.pres_init || xSemaphoreTake(sensor_data.pressureSemaphore,100) == pdFALSE)
	    return false;

	*temp = sensor_data.pTemp;
	bool toReturn = sensor_data.pres_valid;
    xSemaphoreGive(sensor_data.pressureSemaphore);
    return toReturn;
}

bool sens_get_humid(int32_t* humidity) {
	if(!sensor_data.humid_init || xSemaphoreTake(sensor_data.humiditySemaphore, 100) == pdFALSE)
	    return false;

    *humidity = sensor_data.humidity;
    bool toReturn = sensor_data.humid_valid;
    xSemaphoreGive(sensor_data.humiditySemaphore);
    return toReturn;
}


bool sens_get_htemp(int32_t* temp) {
    if(!sensor_data.humid_init || xSemaphoreTake(sensor_data.humiditySemaphore, 100) == pdFALSE)
        return false;

    *temp = sensor_data.hTemp;
    bool toReturn = sensor_data.humid_valid;
    xSemaphoreGive(sensor_data.humiditySemaphore);
    return toReturn;
}
