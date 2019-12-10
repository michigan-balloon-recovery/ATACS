# Buzzer Driver
Buzzer driver for:
1. Controlling when a buzzer is on or off. 

## Library Dependencies
1. FreeRTOS (timing support)

## Hardware Resources
1. GPIO Pin
   1. Buzzer Control Pin: P2.3

## Usage
1. Register `task_buzzer()` with the FreeRTOS kernel (ex. `xTaskCreate(task_buzzer, "buzzer", 128, NULL, 1, NULL);`).
2. Set the time the buzzer spends on in milliseconds by editing the `#define` in `buzzer.h` (ex. `#define BUZZ_ON_MS 1000`).
3. Set the timer the buzzer spends off in milliseconds by editing the `#define` in `buzzer.h` (ex. `#define BUZZ_OFF_MS 30000`).
