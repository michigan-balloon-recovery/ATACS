#ifndef MSP_PADS_H_
#define MSP_PADS_H_

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define DIR_IN 0
#define DIR_OUT 1
#define NUM_PADS 10

struct padpin {
    uint8_t pad;
    uint8_t pin;
    padpin(uint8_t pad, uint8_t pin) : pad(pad), pin(pin) {}
};

namespace MSP_Pads {
    volatile static uint8_t const pinIdx[8] = {
         BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7
    };

    volatile static uint8_t* const padOut[NUM_PADS] = {// P1 = index 0
         &P1OUT, &P2OUT, &P3OUT, &P4OUT, &P5OUT, &P6OUT, &P7OUT, &P8OUT, &P9OUT, &P10OUT
    };

    volatile static uint8_t* const padIn[NUM_PADS] = {
         &P1IN, &P2IN, &P3IN, &P4IN, &P5IN, &P6IN, &P7IN, &P8IN, &P9IN, &P10IN
    };

    volatile static uint8_t* const padSel[NUM_PADS] = {
         &P1SEL, &P2SEL, &P3SEL, &P4SEL, &P5SEL, &P6SEL, &P7SEL, &P8SEL, &P9SEL, &P10SEL
    };

    volatile static uint8_t* const padDir[NUM_PADS] = {
         &P1DIR, &P2DIR, &P3DIR, &P4DIR, &P5DIR, &P6DIR, &P7DIR, &P8DIR, &P9DIR, &P10DIR
    };

    volatile static uint8_t* const padIE[2] = {
         &P1IE, &P2IE
    };

    volatile static uint8_t * const padIFG[2] = {
         &P1IFG, &P2IFG
    };

    volatile static uint8_t* const padREN[NUM_PADS] = {
         &P1REN, &P2REN, &P3REN, &P4REN, &P5REN, &P6REN, &P7REN, &P8REN, &P9REN, &P10REN
    };

    void setDir(padpin p, bool dir);
    bool read(padpin p);
    void write(padpin p, bool val);
};

#endif /* MSP_PADS_H_ */

