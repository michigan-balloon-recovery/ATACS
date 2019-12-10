# Sensor Driver
Sensor driver for:
1. Calculating and retrieving pressure, humidity, and temperature data

## Library Dependencies
1. [FreeRTOS](https://www.freertos.org/index.html) (semaphore and mutex support)
2. [i2c driver](../i2c/README.md) 

## Hardware Resources
1. USCI B0
   1. SDA pin: P3.1
   2. SCL pin: P3.2

## Usage
1. Register task_humidity and task_pressure with the FreeRTOS kernel.
   (ex. xTaskCreate(task_pressure, "pressure", 1024,NULL,1,NULL);)
2. Use sens_init_pressure() to initialize the pressure sensor and read in its calibration values. 
   There is no initialization necessary for the humidity sensor.
3. Use sens_calc_pressure() to initialize a new pressure and temperature reading from the pressure sensor.
4. Use sens_calc_humidity() to initialize a new humidity and temperature reading from the humidity sensor.
5. Use sens_get_pres() to retrieve the current pressure reading.
6. Use sens_get_ptemp() to retrieve the current temperature reading from the pressure sensor.
7. Use sens_get_humid() to retrieve the current humidity reading.
8. Use sens_get_htemp() to retriece the current temperature reading from the humidity sensor.
9. Use functions (sens_disable_interrupts(), sens_enable_interrupts()) to safely allow for critical sections in other regions of code.
