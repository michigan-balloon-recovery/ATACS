/*
 * afsk.h
 *
 *  Created on: Nov 2, 2019
 *      Author: Justin
 */
#include <stdbool.h>

#ifndef AFSF_H_
#define AFSK_H_

/*
 * Exported Function Definitions
 */
void afsk_setup(const uint16_t tx_port, const uint8_t tx_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin);
void afsk_send(const uint8_t* buf, uint16_t len);
void afsk_start();
bool afsk_flush();
void afsk_isr();

#endif /* AFSK_H_ */
