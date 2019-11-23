#include <stdint.h>
#include "ff_headers.h"

//max data points per file
#define maxData 1000
#define maxFileNameLength 15

typedef struct {
    uint16_t num_entries;
    uint8_t current_log [11];
} log_t;

log_t rb_log;
log_t gnss_log;
log_t aprs_log;
log_t sens_log;

void log_init();

void log_rb();

void log_gnss();

void log_sens();

void log_aprs();
