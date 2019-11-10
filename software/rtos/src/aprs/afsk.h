#include <stdbool.h>
#include <FreeRTOSConfig.h> // For configCPU_CLOCK_HZ, could be hard-coded here instead

#ifndef AFSF_H_
#define AFSK_H_

#define AFSK_CLOCKRATE        configCPU_CLOCK_HZ        // SMCLK rate
#define AFSK_CPS              256                       // CPU cycles per sample
#define AFSK_SPS              52                        // Samples per symbol (1200 baud)
#define AFSK_TABLE_SIZE_100   51200                     // Size of LUT *100
#define AFSK_STRIDE_MARK_100  983                       // Indices to move in LUT for mark (1200 Hz)
#define AFSK_STRIDE_SPACE_100 1802                      // Indices to move in LUT for space (2200 Hz)

/*
 * Exported Function Definitions
 */
void afsk_setup(const uint16_t tx_port, const uint8_t tx_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin,
                const uint16_t max_packet_size);
void afsk_reset();
int afsk_packet_append(const uint8_t* buf, uint16_t len);
void afsk_send();
void afsk_test();

#endif /* AFSK_H_ */
