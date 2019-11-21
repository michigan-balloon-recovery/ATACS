#ifndef APRS_H_
#define APRS_H_

#include <stdint.h>
#include "gnss.h"

/*
 * Exported Function Declarations
 */
void aprs_setup(const uint16_t pd_port,  const uint8_t pd_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin,
                const uint16_t tx_port,  const uint8_t tx_pin);

void aprs_beacon(gnss_time_t* time, gnss_coordinate_pair_t* loc, int32_t* alt);

#endif /* APRS_H_ */
