#include <stdint.h>
#include <stdlib.h>
#include <afsk.h>
#include <driverlib.h>

/*
 * Global Constants
 */

const uint8_t AFSK_SINE_TABLE[AFSK_TABLE_SIZE_100/100] = {
    128, 129, 131, 132, 134, 135, 137, 138, 140, 142, 143, 145, 146, 148, 149, 151,
    152, 154, 155, 157, 158, 160, 162, 163, 165, 166, 167, 169, 170, 172, 173, 175,
    176, 178, 179, 181, 182, 183, 185, 186, 188, 189, 190, 192, 193, 194, 196, 197,
    198, 200, 201, 202, 203, 205, 206, 207, 208, 210, 211, 212, 213, 214, 215, 217,
    218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
    234, 234, 235, 236, 237, 238, 238, 239, 240, 241, 241, 242, 243, 243, 244, 245,
    245, 246, 246, 247, 248, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 252,
    253, 253, 253, 253, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 254, 254, 253, 253, 253,
    253, 252, 252, 252, 251, 251, 250, 250, 250, 249, 249, 248, 248, 247, 246, 246,
    245, 245, 244, 243, 243, 242, 241, 241, 240, 239, 238, 238, 237, 236, 235, 234,
    234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 220, 219,
    218, 217, 215, 214, 213, 212, 211, 210, 208, 207, 206, 205, 203, 202, 201, 200,
    198, 197, 196, 194, 193, 192, 190, 189, 188, 186, 185, 183, 182, 181, 179, 178,
    176, 175, 173, 172, 170, 169, 167, 166, 165, 163, 162, 160, 158, 157, 155, 154,
    152, 151, 149, 148, 146, 145, 143, 142, 140, 138, 137, 135, 134, 132, 131, 129,
    128, 126, 124, 123, 121, 120, 118, 117, 115, 113, 112, 110, 109, 107, 106, 104,
    103, 101, 100, 98,  97,  95,  93,  92,  90,  89,  88,  86,  85,  83,  82,  80,
    79,  77,  76,  74,  73,  72,  70,  69,  67,  66,  65,  63,  62,  61,  59,  58,
    57,  55,  54,  53,  52,  50,  49,  48,  47,  45,  44,  43,  42,  41,  40,  38,
    37,  36,  35,  34,  33,  32,  31,  30,  29,  28,  27,  26,  25,  24,  23,  22,
    21,  21,  20,  19,  18,  17,  17,  16,  15,  14,  14,  13,  12,  12,  11,  10,
    10,  9,   9,   8,   7,   7,   6,   6,   5,   5,   5,   4,   4,   3,   3,   3,
    2,   2,   2,   2,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   2,   2,   2,
    2,   3,   3,   3,   4,   4,   5,   5,   5,   6,   6,   7,   7,   8,   9,   9,
    10,  10,  11,  12,  12,  13,  14,  14,  15,  16,  17,  17,  18,  19,  20,  21,
    21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
    37,  38,  40,  41,  42,  43,  44,  45,  47,  48,  49,  50,  52,  53,  54,  55,
    57,  58,  59,  61,  62,  63,  65,  66,  67,  69,  70,  72,  73,  74,  76,  77,
    79,  80,  82,  83,  85,  86,  88,  89,  90,  92,  93,  95,  97,  98,  100, 101,
    103, 104, 106, 107, 109, 110, 112, 113, 115, 117, 118, 120, 121, 123, 124, 126,
};

/*
 * Global State
 */

typedef struct {
    uint16_t ptt_port, tx_port;
    uint8_t  ptt_pin, tx_pin;

    uint16_t sine_idx_100;
    uint16_t stride_100;

    uint8_t  tx_flag;
    uint16_t tx_idx;     // bitwise

    uint8_t* packet_buf;
    uint16_t packet_len; // bitwise
    uint8_t  current_byte;
} afsk_state_t;

afsk_state_t afsk_state;

/*
 * Local Function Declarations
 */
void afsk_timer_setup();
void afsk_timer_start();
void afsk_timer_stop();

/*
 * Exported Functions Definitions
 */
void afsk_setup(const uint16_t tx_port, const uint8_t tx_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin){
    // Setup radio GPIO
    afsk_state.ptt_port = ptt_port;
    afsk_state.ptt_pin = ptt_pin;
    GPIO_setOutputHighOnPin(ptt_port, ptt_pin);
    afsk_state.tx_port = tx_port;
    afsk_state.tx_pin  = tx_pin;
    GPIO_setAsPeripheralModuleFunctionOutputPin(tx_port, tx_pin);

    // Setup timer ISR
    afsk_timer_setup();

    // Reset tx flag
    afsk_state.tx_flag = false;
}

void afsk_reset(){
    if (afsk_state.packet_buf) {
        free(afsk_state.packet_buf);
        afsk_state.packet_buf = 0;
    }
    afsk_state.packet_len = 0;
}

void afsk_send(uint8_t* buf, uint16_t len) {
    afsk_state.packet_buf = buf;
    afsk_state.packet_len = len; //bits
}

void afsk_transmit(){
    if (afsk_state.packet_buf == 0 || afsk_state.packet_len == 0)
        return;

    // Put radio in TX mode (active low), wait 1ms
    GPIO_setOutputHighOnPin(afsk_state.ptt_port, afsk_state.ptt_pin);
    __delay_cycles(configCPU_CLOCK_HZ / 10);

    // Reset metadata
    afsk_state.tx_idx             = 0;
    afsk_state.sine_idx_100       = 0;
    afsk_state.stride_100         = AFSK_STRIDE_SPACE_100;    // initially 2200 Hz
    afsk_state.current_byte       = afsk_state.packet_buf[0];

    // Set tx flag and start timers
    afsk_state.tx_flag            = true;
    afsk_timer_start();

    // Block until ISR resets tx flag
    while(afsk_state.tx_flag);

    // Wait 1ms, then put radio back in RX mode
    __delay_cycles(configCPU_CLOCK_HZ / 1000);
    GPIO_setOutputLowOnPin(afsk_state.ptt_port, afsk_state.ptt_pin);
}

/*
 * Local Function Definitions
 */
void afsk_timer_setup(){
    TA1CCR0   = AFSK_CPS;
    TA1CCTL1 |= OUTMOD_7; // set at CCR0, reset at CCRx
    TA1CCR1   = AFSK_SINE_TABLE[0];
    TA1CTL    = TASSEL__SMCLK | MC__STOP;
}

void afsk_timer_start(){
    TA1CTL   |= MC__UP;
    TA1CCTL0 |= CCIE;
}

void afsk_timer_stop(){
    TA1CTL   &= ~MC__STOP;
    TA1CCTL0 &= ~CCIE;
}

/*
 * AFSK ISR
 */
uint16_t sample_ctr = 0;
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR (void) {
    // Read TAIV to reset interrupt flag
    uint16_t taiv = TA1IV;

    // Adjust duty cycle
    TA1CCR1= AFSK_SINE_TABLE[afsk_state.sine_idx_100/100];
    afsk_state.sine_idx_100 += afsk_state.stride_100;
    if (afsk_state.sine_idx_100 > AFSK_TABLE_SIZE_100) {
        afsk_state.sine_idx_100 -= AFSK_TABLE_SIZE_100;
    }

    sample_ctr++; // Another sample completed

    // Bit finished
    if (sample_ctr == AFSK_SPS) {
        sample_ctr = 0;
        afsk_state.tx_idx++;

        if (afsk_state.tx_idx > afsk_state.packet_len) {
            afsk_state.tx_flag = false;
            afsk_state.packet_len = 0;
            TA1CCR1 = AFSK_CPS/2;
            afsk_timer_stop();
            return;
        }

        if ((afsk_state.tx_idx & 7) == 0) {
            afsk_state.current_byte = afsk_state.packet_buf[afsk_state.tx_idx >> 3];
        } else {
            afsk_state.current_byte /= 2;
        }

        if ((afsk_state.current_byte & 1) == 0) { // if the next bit is 0, toggle the frequency
            afsk_state.stride_100 ^= (AFSK_STRIDE_MARK_100 ^ AFSK_STRIDE_SPACE_100);
        }
    }
}

