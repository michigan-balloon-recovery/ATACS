#ifndef AFSK_H_
#define AFSK_H_

#ifdef __cplusplus
extern "C" {
#endif


// -------------------------------------------------------------- //
// -------------------- include dependencies -------------------- //
// -------------------------------------------------------------- //

// standard libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
// MSP430 hardware
#include <driverlib.h>
// FreeRTOS
#include <FreeRTOSConfig.h> 
// application drivers
#include <afsk.h>


// ------------------------------------------------------- //
// -------------------- public macros -------------------- //
// ------------------------------------------------------- //

#define AFSK_CLOCKRATE        configCPU_CLOCK_HZ        // SMCLK rate
#define AFSK_CPS              256                       // CPU cycles per sample
#define AFSK_SPS              52                        // Samples per symbol (1200 baud)
#define AFSK_TABLE_SIZE_100   51200                     // Size of LUT *100
#define AFSK_STRIDE_MARK_100  983                       // Indices to move in LUT for mark (1200 Hz)
#define AFSK_STRIDE_SPACE_100 1802                      // Indices to move in LUT for space (2200 Hz)

// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/*!
 * \brief Initializes appropriate hardware to enable APRS transmissions
 *
 * @param ptt_port Port to which the radio's PTT pin is connected
 * @param ptt_pin  Pin to which the radio's PTT pin is connected
 * @param tx_port Port to which the radio's MIC pin is connected
 * @param tx_pin  Pin to which the radio's MIC pin is connected
 * @param ptt_active_high Whether activity level (true: transmit when high)
 * \return None
 */
void afsk_setup(const uint16_t ptt_port, const uint8_t ptt_pin,
                const uint16_t tx_port,  const uint8_t tx_pin,
                const bool ptt_active_high);

/*!
 * \brief Clear transmit buffer
 *
 * \return None
 */
void afsk_clear();

/*!
 * \brief Load data into transmit buffer
 * 
 * @param buf Data buffer
 * @param len Length of buf
 * \return None
 */
void afsk_send(uint8_t* buf, uint16_t len);

/*!
 * \brief Trigger AFSK generation from transmit buffer
 *
 * \return None
 */
void afsk_transmit();

#endif /* AFSK_H_ */
