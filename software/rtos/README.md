# ATACS software
## Import instructions for Code Composer Studio 9
1. After installing CCS9, configure it to use a workspace that is **not** the same as the project directory.
2. `File` > `Import` > `Code Composer Studio` > `CCS Projects`
3. Set import source as `ATACS/software/rtos`, then Finish. `rtos` should now appear in the Project Explorer pane
4. Right-click the `rtos` project, go to `Properties`, then `Manage Configurations`
5. Select `Debug_Large_Date_Model` and `Set Active`

## Driver documentation
1. [APRS](./src/aprs/README.md)
2. [Buzzer](./src/buzzer/README.md)
3. [GNSS](./src/gnss/README.md)
4. [I2C](./src/I2C/README.md)
5. [Logging](./src/logging/README.md)
6. [Ring Buffer](./src/ring_buff/README.md)
7. [RockBLOCK](./src/RockBLOCK/README.md)
8. [Sensors](./src/Sensors/README.md)
9. [UART](./src/uart/README.md)
10. [XBee](./src/XBee/README.md)
