#include <custom_spi.h>


void spi_init() {
    // init pins
//    P3OUT |= 0x01 | 0x02 | 0x08;
    P3DIR |= 0x01 | 0x02 | 0x08;
    P3SEL |= 0x02 | 0x04 | 0x08;
    P3DIR &= ~(0x04);
    P3OUT |= 0x01;

    // reset UCB0 module
    UCB0CTL1 |= UCSWRST;

    // set configuration bits
    UCB0CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC;
    UCB0CTL1 &= ~(UCSSEL_1);
    UCB0CTL1 |= UCSSEL__SMCLK;

    // set prescaler
    UCB0BR0 = 0x28;
    UCB0BR1 = 0x00;

    // enable UCB0 module
    UCB0CTL1 &= ~UCSWRST;
}


uint8_t spi_byte(uint8_t datum) {
    // wait for peripheral to finish transactions
    while(UCB0STAT & UCBUSY);
    UCB0RXBUF;
    UCB0TXBUF = datum;
    // wait for data to be received
    while(!(UCB0IFG & UCRXIFG));
    return UCB0RXBUF;
}


