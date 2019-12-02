#include "ff.h"
#include <stdlib.h>
#include "sensors.h"
#include "gnss.h"
#include "rockblock.h"
#include <string.h>
#include <stdint.h>
#include "ff.h"
#include <stdlib.h>
#include "sensors.h"
#include "gnss.h"
#include "rockblock.h"
#include <string.h>

//max data points per file
#define maxData                     5
#define maxFileNameLength           30
#define maxHeaderSize               100

typedef struct {
    uint16_t num_entries;
    char current_log [maxFileNameLength];
    char log_header [maxHeaderSize];
    FSIZE_t fpointer;
} log_t;

log_t rb_log;
log_t gnss_log;
log_t aprs_log;
log_t sens_log;

void task_log();

void log_init();

void log_rb();

void log_gnss();

void log_sens();

void log_aprs();
