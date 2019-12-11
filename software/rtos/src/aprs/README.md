# APRS driver
[APRS (Automatic Packet Reporting System)](https://en.wikipedia.org/wiki/Automatic_Packet_Reporting_System) driver which uses Timer A1 to produce a PWM signal which, after being low-pass filtered, emulates the [AFSK](https://en.wikipedia.org/wiki/Frequency-shift_keying) tones expected by APRS. 

## Library Dependencies
1. FreeRTOS (semaphore and mutex support)
2. [Gustavo Litovsky's UART driver for MSP430](../uart/README.md) (modified to use [Frame-preserving ring buffer](../ring_buff/README.md))
3. [ATACS RockBLOCK](../RockBLOCK/README.md)
4. [ATACS GNSS](../gnss/README.md)
5. [ATACS Sensors](../Sensors/README.md)

## Hardware resources
* USCI A3
   * RX pin: P10.5
   * TT pin: P10.4

* Timer A1
   * CCR1 Output (TX): P2.2

* GPIO Pins
    * DRA818V PD pin:  P1.2
    * DRA818V PTT pin: P1.3

## Usage
* In `aprs.h`
    1. Set APRS source and destination callsigns. For most users, destination should remain as `APRS` and source should be the licensed operator's callsign.
    2. Set APRS comment (optional)
    3. Set APRS transmit period (should not be lower than `60000`)
    4. Set GPIO pins used by the RF module for PD (on/off) and PTT (push-to-talk, transmit enable)
    5. Set APRS transmit active level (active low or high)
* **ADVANCED USERS:** If you would like to modify beacon behavior, see the definition of `task_aprs` and `aprs_beacon` in `aprs.c`