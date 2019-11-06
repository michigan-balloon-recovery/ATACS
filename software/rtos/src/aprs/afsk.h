/*
 * afsk.h
 *
 *  Created on: Nov 2, 2019
 *      Author: Justin
 */
#include <stdbool.h>

#ifndef AFSF_H_
#define AFSK_H_

#define AFSK_CLOCKRATE       configCPU_CLOCK_HZ       // SMCLK rate
#define AFSK_CPS             256                      // Cycles per sample
#define AFSK_TABLE_SIZE_100  51200
#define AFSK_STRIDE_1200_100 983                      //
#define AFSK_STRIDE_2200_100 1802                     //

/*
 * Exported Function Definitions
 */
void afsk_setup(const uint16_t tx_port, const uint8_t tx_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin);
void afsk_send(const uint8_t* buf, uint16_t len);
void afsk_start();
void afsk_stop();
bool afsk_flush();
void afsk_test();

#endif /* AFSK_H_ */
