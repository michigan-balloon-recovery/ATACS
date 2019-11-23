#include "sensors.h"

/**
 * sensors.c
 */

sensor_data_t sensor_data = {.is_valid = false};

void task_pressure(void) {
    const portTickType xFrequency = 1000 / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    sensor_data.pressureSemaphore = xSemaphoreCreateMutex();
    sens_init_pres();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        int32_t data[1];
        sens_calc_pres(data);

        if(xSemaphoreTake(sensor_data.pressureSemaphore,100/portTICK_RATE_MS) == pdTRUE) {
            sensor_data.pressure = data[0];
            sensor_data.pTemp = data[1];
            xSemaphoreGive(sensor_data.pressureSemaphore);
        }
    }
}

void task_humidity(void) {
    const portTickType xFrequency = 1000 / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    sensor_data.humiditySemaphore = xSemaphoreCreateMutex();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        int32_t data[1];
        sens_calc_humid(data);

        if(xSemaphoreTake(sensor_data.humiditySemaphore, 100/portTICK_RATE_MS) == pdTRUE) {
            sensor_data.humidity = data[0];
            sensor_data.hTemp = data[1];
            xSemaphoreGive(sensor_data.humiditySemaphore);
        }
    }
}
 
void sens_init_pres(void) {
	return; //TODO: get pressure sensor to work.
	uint8_t cmd[1];
	
    unsigned int j;
    volatile unsigned int i;
    for(i = 0; i < 8; i++)
        c[i] = 0;

	cmd[0] = 0x1E;
    i2c_write(0x77, cmd, 1);
    for(j = 0; j<8; j++) {
        uint8_t data[2];
        vTaskDelay(100/portTICK_PERIOD_MS);
        cmd[0] = 0xA0 + j;
        i2c_write(0x77, cmd, 1);
        vTaskDelay(100/portTICK_PERIOD_MS);
        i2c_read(0x77, data, 2);
        c[j] = data[0];
        c[j] = c[j] << 8;
        c[j] += data[1];
    }
    sensor_data.is_valid = true;
}
 
void sens_calc_pres(int32_t* return_data) {
    return;
    uint8_t data[4] = {0x0, 0x0, 0x0, 0x0};
    uint8_t cmd[1];

    int32_t pressure = 0;

    uint32_t d1, d2;

    int32_t dT, pTemp;
    int64_t offset, sens;

    //retrieves digital pressure value
    vTaskDelay(100/portTICK_PERIOD_MS);
    cmd[0] = 0x48;
    i2c_write(0x77, cmd, 1);
    vTaskDelay(100/portTICK_PERIOD_MS);
    cmd[0] = 0x00;
    i2c_write(0x77, cmd, 1);
    vTaskDelay(100/portTICK_PERIOD_MS);
    i2c_read(0x77, data, 3);
    d1 = data[0];
    d1 = d1 << 8;
    d1 += data[1];
    d1 = d1 << 8;
    d1 += data[2];

    //retrieves digital temperature value
    cmd[0] = 0x58;
    i2c_write(0x77, cmd, 1);
    vTaskDelay(100/portTICK_PERIOD_MS);
    cmd[0] = 0x00;
    i2c_write(0x77, cmd, 1);
    vTaskDelay(100/portTICK_PERIOD_MS);
    i2c_read(0x77, data, 3);
    d2 = data[0];
    d2 = d2 << 8;
    d2 += data[1];
    d2 = d2 << 8;
    d2 += data[2];

    //calculate temperature
    dT = c[5];
    dT *= 256;
    dT = d2 - dT;

    pTemp = c[6];
    pTemp *= dT;
    pTemp /= 8388608;
    pTemp += 2000;

    //calculate temperature compensated pressure
    int64_t temporary;
    offset = c[2];
    offset *= 65536;
    temporary = c[4];
    temporary += dT;
    temporary /= 128;
    offset = offset + temporary;

    sens = c[1];
    sens *= 32768;
    temporary = c[3];
    temporary *= dT;
    temporary /= 256;


    pressure = d1;
    pressure *= sens;
    pressure /= 2097152;
    pressure -= offset;
    pressure /= 16384; // 32768
    pressure /= 100;

    pTemp += 50;
    pTemp /= 100;
    pTemp *= 1.8;
    pTemp += 32;

    return_data[0] = pressure;
    return_data[1] = pTemp;
}

void sens_calc_humid(int32_t *return_data) {
    uint32_t hum = 0;
    uint8_t data[4] = {0x0, 0x0, 0x0, 0x0};
    int32_t temp = 0;

    i2c_write(0x27, 0, 0);
    vTaskDelay(100/portTICK_PERIOD_MS);
    i2c_read(0x27, data, 4);

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
}

bool sens_get_pres(int32_t* pressure) {
    return false;
	if(xSemaphoreTake(sensor_data.pressureSemaphore,100) == pdTRUE) {
		*pressure = sensor_data.pressure;
		xSemaphoreGive(sensor_data.pressureSemaphore);
		return true;
	}
	
	return false;
}

bool sens_get_ptemp(int32_t* temp) {
    return false;
	if(xSemaphoreTake(sensor_data.pressureSemaphore,100) == pdTRUE)
	{
		*temp = sensor_data.pTemp;
		xSemaphoreGive(sensor_data.pressureSemaphore);
		return true;
	}
	
	return false; 
}

bool sens_get_humid(int32_t* humidity) {
	if(xSemaphoreTake(sensor_data.humiditySemaphore, 100) == pdTRUE) {
		*humidity = sensor_data.humidity;
		xSemaphoreGive(sensor_data.humiditySemaphore);
		return true;
	}

	return false; 
}

bool sens_get_htemp(int32_t* temp) {
    if(xSemaphoreTake(sensor_data.humiditySemaphore, 100) == pdTRUE) {
		*temp = sensor_data.hTemp;
		xSemaphoreGive(sensor_data.humiditySemaphore);
		return true; 
	}

    return false;
}
 

