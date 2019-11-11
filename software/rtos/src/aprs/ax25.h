#include <stdint.h>

#ifndef AFSF_H_
#define AFSK_H_


/*
 * Defines
 */
#define AX25_MAX_PACKET  512     // bytes
#define AX25_FLAG        0x7E
#define AX25_CONTROL     0x03
#define AX25_PROTOCOL    0xF0
#define AX25_CRC_INITIAL 0x8FFF
#define AX25_CRC_POLY    0x8408  // assumes lsb-first computation
#define AX25_TX_DELAY_MS 100

typedef struct {
    char    callsign[7];
    uint8_t ssid;
} address_t;

/*
 * Exported Function Declarations
 */
void ax25_send_header(const address_t* addresses, uint8_t num);
void ax25_send_byte(uint8_t byte);
void ax25_send_string(const uint8_t* buf, const uint16_t buflen);
void ax25_send_footer();
void ax25_flush_frame();

#endif /* AFSK_H_ */
