#ifndef AX25_H_
#define AX25_H_

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------- //
// -------------------- include dependencies -------------------- //
// -------------------------------------------------------------- //

// standard libraries
#include <stdint.h>
// MSP430 hardware
#include <driverlib.h>
// application drivers
#include <afsk.h>


// ------------------------------------------------------- //
// -------------------- public macros -------------------- //
// ------------------------------------------------------- //

#define AX25_MAX_PACKET  512     // bytes
#define AX25_FLAG        0x7E
#define AX25_CONTROL     0x03
#define AX25_PROTOCOL    0xF0
#define AX25_CRC_INITIAL 0xFFFF
#define AX25_CRC_POLY    0x8408  // lsb-first
#define AX25_TX_DELAY_MS 300


// ---------------------------------------------------------- //
// -------------------- type definitions -------------------- //
// ---------------------------------------------------------- //

typedef struct {
    char    callsign[7];
    uint8_t ssid;
} address_t;

typedef struct {
    uint16_t crc;
    uint8_t  cont_ones;
    uint8_t  packet[AX25_MAX_PACKET];
    uint16_t packet_len; //bits

    // For debugging
    uint8_t  raw_packet[AX25_MAX_PACKET];
    uint16_t raw_len; //bytes
} ax25_state_t;

// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/*!
 * \brief Loads AX.25 header into transmit buffer
 *
 * @param addresses Array of address_t which includes the destination and source addresses
 * @param num Length of addresses array
 * \return None
 */
void ax25_send_header(const address_t* addresses, uint8_t num);

/*!
 * \brief Loads byte into transmit buffer
 *
 * @param byte Byte to sent
 * \return None
 */
void ax25_send_byte(const char byte);

/*!
 * \brief Loads string into transmit buffer
 *
 * @param buf Pointer to input string
 * \return None
 */
void ax25_send_string(const char* buf);

/*!
 * \brief Loads AX.25 footer into transmit buffer
 *
 * \return None
 */
void ax25_send_footer();

/*!
 * \brief Send transmit buffer to AFSK driver
 *
 * \return None
 */
void ax25_flush_frame();

#endif /* AX25_H_ */
