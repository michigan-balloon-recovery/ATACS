# I2C Driver
I2C (Inter-Integrated Circuits) driver for:
1. Sending messages from a microcontroller to I2C devices
2. Retrieving messages from I2C devices 

## Library Dependencies
1. [FreeRTOS](https://www.freertos.org/index.html) (semaphore and mutex support)

## Hardware Resources
1. USCI B0
   1. SDA: P3.1
   2. SCL: P3.2

## Usage
1. Set buffer lengths to desired sizes in i2c_driver.c
2. Use i2c_setup() to initialize I2C communication on UCB0
3. Use i2c_write() to send data from the microprocessor to an I2C device. 
4. Use i2c_read() to tell the microporccesor retrieve data from an I2C device. 
5. Use interrupt functions (i2c_enable_interrupts(), i2c_disable_interrupts()) to allow critical sections in other regions of code.

