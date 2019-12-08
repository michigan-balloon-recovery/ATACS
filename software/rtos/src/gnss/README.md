# GNSS driver
GNSS (Global Navigation Satellite System) driver for:
1. generic modules through NMEA (`./NMEA.c`)
2. UBlox modules through UBX (not yet implemented)

## Library Dependencies
1. FreeRTOS (semaphore and mutex support)
2. [Gustavo Litovsky's UART driver for MSP430](../uart/README.md) (modified to use [Frame-preserving ring buffer](../ring_buff/README.md))

## Hardware resources
1. USCI A0
   1. Rx pin: P3.4
   2. Tx pin: P3.5

## Usage
1. select between NMEA and UBX using the macro in `./GNSS.h` (i.e. `#define GNSS_NMEA` or `#define GNSS_UBX`)
2. set buffer lengths to desired sizes in `./GNSS.h`
3. add extra message decoders and get functions to `./NMEA.c` or `./UBX.c` if more data is necessary (such as expected error)
4. register `task_gnss()` with the FreeRTOS kernel (ex. `xTaskCreate(task_aprs, "aprs", 512, NULL, 1, NULL);`)
5. use get functions (`gnss_get_time()`, `gnss_get_location()`, `gnss_get_altitude()`, etc.) to retrieve GNSS data
6. use interrupt functions (`gnss_disable_interrupts()`, `gnss_enable_interrupts()`) to allow critical sections in other regions of code

## Adding NMEA decoders
The NMEA decoding framework is designed to allow modular additions of sentence decoders depending on application. 
Refer to the NMEA spec or your GNSS module's datasheet to decide which sentences to decode.
1. **Add field decoder functions as necessary**: All NMEA sentences are encoded in ASCII so functions must be written to convert each field to a binary representation depending on what the contents of the field is. The function must follow this prototype: `static bool gnss_nmea_field_custom(uint8_t *start, uint8_t *end, void *data)` where `start` is a pointer to the beginning of the field and `end` is a pointer to the end of the field. `data` is a void pointer that can be used to return the decoded data.
2. **Add field decoder prototype**: add the function prototype for the field decoder in the *private prototypes* section at the top of `./NMEA.c`.
3. **decode all desired fields in the desired sentence case**: 'gnss_nmea_decode_standard_msg' and 'gnss_nmea_decode_PUBX' contain switch cases to sort the decoded sentences by ID. Add `gnss_nmea_decode_field()` calls with a function pointer to your custom field decoder to decode any desired fields in the case of the sentence you want to decode. The *GGA* example can be used as reference.
4. **store any new data in the GNSS global struct**: modify `./GNSS.h` to include your new data in the *gnss_t* type.
5. **implement get functions for any new data made available**: add custom get functions to `./GNSS.c` to retreive your new data. Ensure checks are made that the global struct has been initialized and the data mutex is available (ex. `gnss_obj->is_valid && xSemaphoreTake(gnss_obj->data_mutex, 100) == pdFALSE`).
