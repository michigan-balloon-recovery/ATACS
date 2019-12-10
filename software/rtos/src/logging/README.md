# Logging
Task for periodic logging and API for custom, event based logging.

## Library Dependencies
1. [FreeRTOS](https://www.freertos.org/index.html) (semaphore and mutex support)
2. [FatFs](http://elm-chan.org/fsw/ff/00index_e.html) fat32 file system
3. SPI driver from [Texas Instruments MSP430 MMC driver](http://www.ti.com/lit/an/slaa281c/slaa281c.pdf)

## Hardware Resources
1. USCI B3
   1. CS: P10.0
   2. SIMO: P10.1
   3. SOMI: P10.2
   4. CLK: P10.3
   
## Usage: Periodic Logging
1. Configure logging file sizes, period, etc in `./logging.h` **NOTE:** the most recent log will be unreadable if the logging is interrupted (device turned off, SD card removed) while writing. Size your logs small enough to avoid this.
2. Modify `log_init()` to start any custom logging sessions or change log file names/paths
3. Add any custom logging sessions to the `task_log()` function
   1. Call `log_resume_session()` before writing to the SD card
   2. Check that `log_resume_session()` returns true before writing anything to the file. This prevents two tasks from writing to the file at the same time and ensure the file is opened correctly.
   3. Write to the SD card using the file pointer returned by `log_resume_session()`
   4. Call `log_pause_session()` after the current entry has been written to the SD card
3. Register `task_log()` with the FreeRTOS kernel (ex. `xTaskCreate(task_log, "Logging", 512, NULL, 1, NULL);`)

## Usage: Event Based Logging
1. Configure logging file sizes, period, etc in `./logging.h` **NOTE:** the most recent log will be unreadable if the logging is interrupted (device turned off, SD card removed) while writing. Size your logs small enough to avoid this.
2. Modify `log_init()` to start any custom logging sessions or change log file names/paths
3. Create custom logging functions (ex. `log_gnss()`) for every device to log
   1. Call `log_resume_session()` before writing to the SD card
   2. Check that `log_resume_session()` returns true before writing anything to the file. This prevents two tasks from writing to the file at the same time and ensure the file is opened correctly.
   3. Write to the SD card using the file pointer returned by `log_resume_session()`
   4. Call `log_pause_session()` after the current entry has been written to the SD card
4. Call custom logging functions from any other FreeRTOS task. Logs are not synchronized with this method, so ensure to same timestamps or other contextual info.

## Log Format
The logging driver can theoretically be used to write in any format, but by default logs in Comma Seperated Value (CSV) format which can be read using Excel or Matlab.
Every file is writen with a custom header specified when the logging session is created with `log_start_session()`.
The logging driver will then allow `LOG_MAX_ENTRIES` to be written to a log file before incrementing the file name. 
The initial file name is specified when calling `log_start_session()` and must end in a numeric sequence with enough leading zeros to prevent increasing the string length.
