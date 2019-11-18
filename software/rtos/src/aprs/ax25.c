#include <ax25.h>
#include <afsk.h>
#include <driverlib.h>

/*
 * Global State
 */
typedef struct {
    uint16_t crc;
    uint8_t  cont_ones;
    uint8_t  packet[AX25_MAX_PACKET];
    uint16_t packet_len; //bits
} ax25_state_t;

ax25_state_t ax25_state;

/*
 * Local Function Definitions
 */
void update_crc(uint8_t bit){
    ax25_state.crc ^= bit;
    if (ax25_state.crc & 1){
        ax25_state.crc = (ax25_state.crc >> 1) ^ AX25_CRC_POLY;
    } else {
        ax25_state.crc >>= 1;
    }
}

void send_byte(uint8_t byte) {
    uint8_t i = 0;
    for (i = 0 ; i < 8 ; i++) {
        uint8_t bit = byte & 1;
        update_crc(bit);
        byte = byte >> 1;

        if (bit) {
            if (ax25_state.packet_len >= AX25_MAX_PACKET * 8) // Prevent buffer overrun
                return;

            // set (packet_size % 8)th bit of packet[packet_size/8]
            ax25_state.packet[ax25_state.packet_len >> 3] |= (1 << (ax25_state.packet_len & 7));
            ax25_state.packet_len++;

            ax25_state.cont_ones++;
            if (ax25_state.cont_ones < 5)
                continue;
        }

        // Next bit is 0 or zero padding after 5 contiguous 1s
        if (ax25_state.packet_len >= AX25_MAX_PACKET * 8)    // Prevent buffer overrun
            return;

        // reset (packet_size % 8)th bit of packet[packet_size/8]
        ax25_state.packet[ax25_state.packet_len >> 3] &= ~(1 << (ax25_state.packet_len & 7));
        ax25_state.packet_len++;
        ax25_state.cont_ones = 0;
    }
}

void send_flag(){
    // Basically the same as send_byte(AX25_FLAG), but without CRC updates
    uint8_t i;
    for (i = 0 ; i < 8 ; i++) {
        if (ax25_state.packet_len >= (AX25_MAX_PACKET * 8))
            return;

        if ((AX25_FLAG >> i) & 1) {
            // set (packet_size % 8)th bit of packet[packet_size/8]
            ax25_state.packet[ax25_state.packet_len >> 3] |= (1 << (ax25_state.packet_len & 7));
        } else {
            // reset (packet_size % 8)th bit of packet[packet_size/8]
            ax25_state.packet[ax25_state.packet_len >> 3] &= ~(1 << (ax25_state.packet_len & 7));
        }
        ax25_state.packet_len++;
    }
}

/*
 * Exported Functions Definitions
 */
void ax25_send_header(const address_t* addresses, uint8_t num){
    ax25_state.crc = AX25_CRC_INITIAL;
    ax25_state.cont_ones = 0;
    ax25_state.packet_len = 0;

    // Start flag(s)
    uint8_t i;
//    send_flag();
    for(i = 0 ; i < AX25_TX_DELAY_MS/10 * 12 / 8 ; i++){
        // Enough flags to fill AX_TX_DELAY_MS
        send_flag();
    }

    // Addresses (Destination, Source, Digipeaters)
    for(i = 0 ; i < num ; i++){
        // Callsign
        uint8_t j;
        for(j = 0 ; addresses[i].callsign[j] ; j++) {
            send_byte(addresses[i].callsign[j] << 1);
        }

        // Padding
        for( ; j < 6 ; j++) {
            send_byte(' ' << 1);
        }

        // SSID
        if (i == num - 1) {
            // Trailing 1 terminator
            send_byte(('0' + addresses[i].ssid) << 1 | 1);
        } else {
            send_byte(('0' + addresses[i].ssid) << 1);
        }
    }

    // Control Field (UI)
    send_byte(AX25_CONTROL);

    // Protocol ID
    send_byte(AX25_PROTOCOL);
}

void ax25_send_byte(uint8_t byte){
    send_byte(byte);
}

void ax25_send_string(const uint8_t* buf, const uint16_t buflen){
    uint8_t i;
    for(i = 0 ; i < buflen ; i++){
        send_byte(buf[i]);
    }
}

void ax25_send_footer(){
    uint16_t final_crc = ax25_state.crc;
    send_byte(~(final_crc & 0xFF));
    final_crc >>= 8;
    send_byte(~(final_crc & 0xFF));
    send_flag();
}

void ax25_flush_frame(){
    // P2.2 TX, P2.0 PTT
    afsk_setup(GPIO_PORT_P2, GPIO_PIN2, GPIO_PORT_P2, GPIO_PIN0);
    afsk_send(ax25_state.packet, ax25_state.packet_len);
    afsk_transmit();
}
