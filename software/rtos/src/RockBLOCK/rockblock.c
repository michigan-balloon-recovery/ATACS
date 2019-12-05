// Author: Scott Smith


#include "rockblock.h"
#include "xbee.h"

extern gnss_t GNSS;
extern sensor_data_t sensor_data;
extern XBEE_t XBee;

ROCKBLOCK_t rb = {.is_valid = false}; // global rockblock object for the task.

// formats the command for the message sent to the RockBLOCK.
static void rb_format_command(ROCKBLOCK_t *rb, rb_message_t cmd, volatile uint8_t *numReturns) {

    *(rb->tx.cur_ptr++) = 'A';
    *(rb->tx.cur_ptr++) = 'T';

    if(cmd != AT && cmd != ATK0) {
        *(rb->tx.cur_ptr++) = '+';
        *(rb->tx.cur_ptr++) = 'S';
        *(rb->tx.cur_ptr++) = 'B';
        *(rb->tx.cur_ptr++) = 'D';
    }

    switch(cmd) {
        case AT: // are you alive?
            *numReturns = 3;
        break;
        case ATK0: // turn off flow control
            *(rb->tx.cur_ptr++) = '&';
            *(rb->tx.cur_ptr++) = 'K';
            *(rb->tx.cur_ptr++) = '0';
            *numReturns = 3;
        break;
        case SBDWT: // create message
            *(rb->tx.cur_ptr++) = 'W';
            *(rb->tx.cur_ptr++) = 'T';
            *numReturns = 3;
        break;
        case SBDIX: // create session
            *(rb->tx.cur_ptr++) = 'I';
            *(rb->tx.cur_ptr++) = 'X';
            *numReturns = 5;
        break;
        case SBDRT: // download ASCII message
            *(rb->tx.cur_ptr++) = 'R';
            *(rb->tx.cur_ptr++) = 'T';
            *numReturns = 4;
        break;
        default:
            *numReturns = 0;
        break;

    }
    // set end of command index and add the carriage return or equal sign depending on command.
    rb->tx.last_ptr = rb->tx.cur_ptr;

    if(cmd == SBDWT)
        *(rb->tx.cur_ptr++) = '='; // want an '=' sign here, after this the message will follow.
    else
        *(rb->tx.cur_ptr++) = '\r'; // should be 0x0D, same as '\r' hopefully.
}

static void rb_clear_buffers(ROCKBLOCK_t *rb) {
    rb->tx.cur_ptr = rb->tx.buff;
    rb->tx.last_ptr = rb->tx.buff;
    rb->rx.cur_ptr = rb->rx.buff;
    rb->rx.last_ptr = rb->rx.buff;
    rb->tx.tx_ptr = rb->tx.buff;
}

static bool rb_use_uart(ROCKBLOCK_t *rb) {

    if(xSemaphoreTake(rb->busy_semaphore, 2000 / portTICK_RATE_MS) == pdFALSE)
        return false;

    uint16_t totalLen = rb->tx.last_ptr - rb->tx.buff + 1; // length
    rb->rx.finished = false;

    uartSendDataInt(&USCI_A1_cnf, rb->tx.buff, totalLen);

    if(xSemaphoreTake(rb->tx.txSemaphore, 20000 / portTICK_RATE_MS) == pdFALSE) {
        xSemaphoreGive(rb->busy_semaphore);
        return false;
    }

    if(xSemaphoreTake(rb->rx.rxSemaphore, 20000 / portTICK_RATE_MS) == pdFALSE) {
        xSemaphoreGive(rb->busy_semaphore);
        return false;
    }

    xSemaphoreGive(rb->busy_semaphore);
    return true;
}

static void put_int32_array(int32_t toInsert, uint8_t *msg, uint16_t *cur_idx, bool success) {
    char str[15]; // 2^32 + 1 < 15 indexes, so should be able to fit entire int32_t inside of this.
    uint16_t lenStr;
    if(success) {
        ltoa(toInsert, str);
        lenStr = strlen(str);
        memcpy(msg + *cur_idx, str, lenStr);
        *cur_idx += lenStr;
    } else {
        msg[*cur_idx] = '?';
        *cur_idx = *cur_idx + 1;
    }
    msg[*cur_idx] = ',';
    *cur_idx = *cur_idx + 1;
}


void task_rockblock(void) {
    const portTickType xTaskFrequency =  (uint16_t) ((uint32_t) RB_TRANSMIT_RATE_MS / (uint32_t) portTICK_RATE_MS);
    const portTickType xRetryFrequency = RB_RETRY_RATE_MS / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    uint8_t msg[RB_TX_SIZE];
    uint16_t len = 0;
    bool msgSent = false;
    int8_t msgReceived = 0;
    int8_t msgsQueued = 0;
    uint8_t numRetries = 0;
    uint8_t i = 0;

    int32_t pressure, humidity, hTemp, pTemp;
    int32_t altitude;
    gnss_time_t time;
    gnss_coordinate_pair_t location;
    bool success[7];

    for(i = 0; i < 7; i++)
        success[i] = true;

    rb_init(&rb);

    // wait for all sensors to initialize.
    while(!sensor_data.humid_init);
    while(!sensor_data.pres_init);
    while(!GNSS.is_valid);

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xTaskFrequency);

        i = 0;

        success[i++] = sens_get_pres(&pressure);
        success[i++] = sens_get_humid(&humidity);
        success[i++] = sens_get_htemp(&hTemp);
        success[i++] = sens_get_ptemp(&pTemp);
        success[i++] = gnss_get_altitude(&GNSS, &altitude);
        success[i++] = gnss_get_time(&GNSS, &time);
        success[i++] = gnss_get_location(&GNSS, &location);

        rb_create_telemetry_packet(msg, &len, pressure, humidity, pTemp, hTemp, altitude, &time, &location, success);

        msgSent = false;
        msgReceived = 0;
        msgsQueued = 0;
        numRetries = 0;

        rb_send_message(&rb, msg, len, &msgSent, &msgReceived, &msgsQueued);

        while(!msgSent && numRetries < RB_MAX_TX_RETRIES) {
            numRetries++;
            vTaskDelay(xRetryFrequency);
            rb_start_session(&rb, &msgSent, &msgReceived, &msgsQueued);
        }

        numRetries = 0;

        if(msgReceived == 1) { // we received a message
            if(rb_retrieve_message(&rb))
                rb_process_message(&rb.rx);

            while(msgsQueued > 0 && numRetries < RB_MAX_RX_RETRIES ) { // other messages to download
                rb_start_session(&rb, &msgSent, &msgReceived, &msgsQueued);
                if(msgReceived == 1) {
                    numRetries = 0;
                    if(rb_retrieve_message(&rb))
                        rb_process_message(&rb.rx);

                } else {
                    numRetries++;
                    vTaskDelay(xRetryFrequency);
                }
            }
        }
        xLastWakeTime = xTaskGetTickCount();
    }

}

void rb_init(ROCKBLOCK_t *rb) {

    // initialize buffers with appropriate start values and end values.
    rb->rx.cur_ptr = rb->rx.buff;
    rb->rx.last_ptr = rb->rx.buff;

    rb->tx.cur_ptr = rb->tx.buff;
    rb->tx.last_ptr = rb->tx.buff;

    rb->rx.rxSemaphore = xSemaphoreCreateCounting(1, 0);
    rb->tx.txSemaphore = xSemaphoreCreateCounting(1, 0);
    rb->busy_semaphore = xSemaphoreCreateMutex();

    // UART initialization
    UARTConfig a1_cnf = {
                    .moduleName = USCI_A1,
                    .portNum = PORT_5,
                    .RxPinNum = PIN7,
                    .TxPinNum = PIN6,
                    .clkRate = configCPU_CLOCK_HZ,
                    .baudRate = 19200L,
                    .clkSrc = UART_CLK_SRC_SMCLK,
                    .databits = 8,
                    .parity = UART_PARITY_NONE,
                    .stopbits = 1
    };

    // not using ring buffer, so these are null. All TX/RX choices are processed by callback function in this file
    initUSCIUart(&a1_cnf, NULL, NULL);
    initUartRxCallback(&USCI_A1_cnf, &rb_rx_callback, rb);
    initUartTxCallback(&USCI_A1_cnf, &rb_tx_callback, rb);

    // ring, network-available, and sleep pin initialization
    P8DIR &= ~(BIT0 | BIT1); // set ring and network-available pins to inputs. ON OUR MSP430
    P8DIR |= BIT5;
    // P8DIR &= ~(BIT0 | BIT2); // OLIMEX ring, netav pins

    P7DIR |= BIT3; // set sleep to an output.

    rb_set_awake(true); // TODO: might want to not just always be on, so add the ability to sleep...
    //rb_set_awake(false);
    rb->is_valid = true;
}


void rb_set_awake(bool awake) {
    if(awake)
        P7OUT |= BIT3; // set 7.3 high
    else
        P7OUT &= ~BIT3; // set 7.3 low
}

bool rb_check_ring(void) {
    if(P8IN & BIT0) // get all input statuses on port 8, and check specifically bit0 (RI)
        return false; // RI is active low, so if the pin is high we must not have a ring.
    else
        return true; // it was 0 (Active low), so we have a ring.
}

bool rb_check_netav(void){
    if(P8IN & BIT1) // checking P8.1, which is where NETAV pin is connected.
        return true; // this pin is active high.
    else
        return false; // no signal at the time.
}

void rb_rx_callback(void *param, uint8_t datum) {
    static uint8_t numReturns = 0;
    ROCKBLOCK_t *rb = (ROCKBLOCK_t *) param;

    if(rb->rx.finished == false) {
        *(rb->rx.cur_ptr) = datum; // take the data, we assume we have room here.

        if( (datum == (uint8_t) '\r') || (datum == (uint8_t) ')')) { // all message responses end with '\r', but there might be multiple '\r' per message.
            numReturns++;

            if(numReturns == rb->rx.numReturns) {
                numReturns = 0;
                rb->rx.finished = true;
                rb->rx.last_ptr = rb->rx.cur_ptr;
                xSemaphoreGiveFromISR(rb->rx.rxSemaphore, NULL);
            }
        }

//        if(datum == (uint8_t) ')' || datum == (uint8_t) '(')
//            numReturns = 200;

        rb->rx.cur_ptr++; // increment the index.
    }
}

bool rb_tx_callback(void *param, uint8_t *txAddress) {
    ROCKBLOCK_t *rb = (ROCKBLOCK_t *) param;

    if(rb->tx.tx_ptr > rb->tx.last_ptr) { // sent last byte.
        xSemaphoreGiveFromISR(rb->tx.txSemaphore, NULL);
        return false;
    }

    *txAddress= *(rb->tx.tx_ptr);

    rb->tx.tx_ptr++;
    return true;
}

void rb_send_message(ROCKBLOCK_t *rb, uint8_t *msg, uint16_t len, bool *msgSent, int8_t *msgReceived, int8_t *msgsQueued) {

    rb_clear_buffers(rb);

    // create the message.
    rb_format_command(rb, SBDWT, &(rb->rx.numReturns));    // Set up the command

    uint16_t i = 0;
    for(i = 0; i < len; i++) {
        *(rb->tx.cur_ptr++) = *(msg+i);
    }

    rb->tx.last_ptr = rb->tx.cur_ptr;
    *(rb->tx.cur_ptr++) = '\r'; // end message with carriage return.
    rb->tx.last_ptr = rb->tx.cur_ptr;
    *(rb->tx.cur_ptr++) = '\r'; // end message with carriage return.

    if(rb_use_uart(rb) == false) {
        *msgSent = false;
        *msgReceived = 0;
        *msgsQueued = 0;
        return;
    }

    rb_start_session(rb, msgSent, msgReceived, msgsQueued);

    // process response to see if message succeeded.

}

void rb_start_session(ROCKBLOCK_t *rb, bool *msgSent, int8_t *msgReceived, int8_t *msgsQueued) {
    // start session.

    rb_clear_buffers(rb);
    rb_format_command(rb, SBDIX, &(rb->rx.numReturns));
    uint16_t totalLen;

    if(rb_use_uart(rb) == false) {
        *msgSent = false;
        *msgReceived = 0;
        *msgsQueued = 0;
        return;
    }

    totalLen = rb->rx.last_ptr - rb->rx.buff + 1;

    *msgSent = true;
    *msgReceived = 0;

    // response format:
    //+SBDIX: <MO status>, <MOMSN>, <MT status>, <MTMSN>, <MT length>, <MT queued>\r

    uint16_t i = 0;
    uint8_t numField = 0;
    uint8_t numDigits = 0;

    for(i = 0; i < totalLen; i++) {
        switch(numField) {
            case 0: // SBDIX header
                if(rb->rx.buff[i] == ':')
                    numField++;
            break;
            case 1: // msgSent (MO). Should be 0, 1, or 2.
                if(rb->rx.buff[i] == ' ')
                    continue;
                else if(rb->rx.buff[i] == ',') {
                    numDigits = 0;
                    numField++;
                } else if(rb->rx.buff[i] == '0' || rb->rx.buff[i] == '1' || rb->rx.buff[i] == '2') {
                    if(numDigits == 0) {
                        *msgSent = true;
                        numDigits = 1;
                    } else {
                        *msgSent = false;
                        numDigits = 1;
                    }
                } else { // received anything weird, we failed.
                    *msgSent = false;
                    numDigits = 1; //if its greater than 0, and we go through the loop again, msgSent can't go true. Which is what we want.
                }
            break;
            case 2: // don't care.
                if(rb->rx.buff[i] == ',')
                    numField++;
                else
                    continue;
            break;
            case 3: // receive status (MT). 0 -> no message received, 1 -> message received, 2 -> error.
                if(rb->rx.buff[i] == ',')
                    numField++;
                else if(rb->rx.buff[i] == ' ')
                    continue;
                else if(rb->rx.buff[i] == '0')
                    *msgReceived = 0;
                else if(rb->rx.buff[i] == '1')
                    *msgReceived = 1;
                else // we are using -1 as an error output from this function. The number 2 in this field from the message means an error, though.
                    *msgReceived = -1;
            break;
            case 4: //don't care
                if(rb->rx.buff[i] == ',')
                    numField++;
                else
                    continue;
            break;
            case 5: // size of downloaded message, don't care currently.
                if(rb->rx.buff[i] == ',')
                    numField++;
                else
                    continue;
            break;
            case 6: // number of messages in queue waiting to be downloaded.
                if(*msgReceived == -1 || *msgReceived == 0)
                    continue;
                else {
                    int j = i;
                    for(j = i; j < totalLen; j++) {
                        if(rb->rx.buff[i] == '\r' || j == totalLen - 1) {
                            rb->rx.buff[i] = '\0';
                            break;
                        }
                    }
                    *msgsQueued = atoi((const char*) rb->rx.buff + i);
                    i = totalLen;
                }
            break;
        }
    }
}

uint8_t rb_check_mailbox(ROCKBLOCK_t *rb, int8_t *msgsQueued) {
    bool msgSent;
    int8_t msgReceived;
    rb_clear_buffers(rb);
    rb_send_message(rb, NULL, 0, &msgSent, &msgReceived, msgsQueued);
    return msgReceived;
}

bool rb_retrieve_message(ROCKBLOCK_t *rb) {

    rb_clear_buffers(rb);
    rb_format_command(rb, SBDRT, &(rb->rx.numReturns));
    return rb_use_uart(rb);

}

void rb_create_telemetry_packet(uint8_t *msg, uint16_t *len, int32_t pressure,
                       int32_t humidity, int32_t pTemp, int32_t hTemp, int32_t altitude,
                       gnss_time_t *time, gnss_coordinate_pair_t *location, bool *success)
{
    uint16_t cur_idx = 0;
    char str[15];

    size_t lenstr = strlen("ATACS,");
    memcpy(msg, "ATACS,", lenstr); // header len = 6
    cur_idx += lenstr;

    put_int32_array(pressure, msg, &cur_idx, success[0]);
    put_int32_array(humidity, msg, &cur_idx, success[1]);
    put_int32_array(hTemp, msg, &cur_idx, success[2]);
    put_int32_array(pTemp, msg, &cur_idx, success[3]);
    put_int32_array(altitude, msg, &cur_idx, success[4]);

    if(success[5]) { // time
        ltoa(time->hour, str);
        lenstr = strlen(str);
        memcpy(msg+cur_idx, str, lenstr);
        cur_idx += lenstr;
        msg[cur_idx++] = ':';
        ltoa(time->min, str);
        lenstr = strlen(str);
        memcpy(msg+cur_idx, str, lenstr);
        cur_idx += lenstr;
        msg[cur_idx++] = ',';
    } else {
        msg[cur_idx++] = '?';
        msg[cur_idx++] = ',';
    }

    if(success[6]) {// location
//        int32_t decSecLat = gnss_coord_to_decMilliSec(&location->latitude); // should be fine using signed integer here.
//        int32_t decSecLong = gnss_coord_to_decMilliSec(&location->longitude);

        put_int32_array(location->latitude.decMilliSec, msg, &cur_idx, true);
        msg[cur_idx++] = location->latitude.dir;
        msg[cur_idx++] = ',';
        put_int32_array(location->longitude.decMilliSec, msg, &cur_idx, true);
        msg[cur_idx++] = location->longitude.dir;
        msg[cur_idx++] = '\r'; // end of message
    } else {
        msg[cur_idx++] = '?'; // lat
        msg[cur_idx++] = ',';
        msg[cur_idx++] = '?'; // lat dir
        msg[cur_idx++] = ',';
        msg[cur_idx++] = '?'; // long
        msg[cur_idx++] = ',';
        msg[cur_idx++] = '?'; // long dir
        msg[cur_idx++] = '\r'; // end of message
    }
    *len = cur_idx;
}

bool rb_process_message(rb_rx_buffer_t *rx) {

//    uint16_t len = rx->last_ptr - rx->buff;
//    uint16_t cur_idx = 0;
//    int i = 0;
//    for(i = 0; i < len; i++) {
//        if(rx->buff[i] == '\n') { // our data should be in the spot after the \n
//            cur_idx = i+1;
//        }
//    }
//
//    char msg[] = "FTUPLZ";
//    bool cut = true;
//    for(i = 0; i < strlen("FTUPLZ"); i++) {
//        if(msg[i] != rx->buff[cur_idx++]) {
//            cut = false;
//            break;
//        }
//    }
    int i = 0;
    bool cut = true;

    if(cut) {
        P8OUT |= BIT3;
        __bic_SR_register(GIE); // critical section for safety!
        rb_cut_ftu(true);
        for(i = 0; i < 20; i++) {
            __delay_cycles(16000000UL);
        }
        rb_cut_ftu(false);
        __bis_SR_register(GIE);
        P8OUT &= ~BIT3;
    }

    return cut;

    if(rx->buff[cur_idx++] != RB_SOF)
        return false;

    if(rx->buff[cur_idx++] != 0) { // use xbee and send this message
        xb_transmit(&XBee, rx->buff + cur_idx, rx->last_ptr - (rx->buff + cur_idx));
        return true;
    }

    switch(rx->buff[cur_idx]) {
    case CUT_FTU_NOW:
        __bic_SR_register(GIE); // critical section for safety!
        rb_cut_ftu(true);
        for(i = 0; i < 20; i++) {
            __delay_cycles(16000000);
        }
        rb_cut_ftu(false);
        __bis_SR_register(GIE);
        break;
    case GET_TELEM:
        break;
    case CONFIG_BUZZER:
        break;
    case SET_FTU_TIMER:
        break;
    case START_FTU_TIMER:
        break;
    case STOP_FTU_TIMER:
        break;
    }
    return false;
}

void rb_cut_ftu(bool cut) {
    if(cut)
        P8OUT |= BIT5;
    else
        P8OUT &= ~BIT5;
}

void rb_enable_interrupts(ROCKBLOCK_t *rb) {

    enableUartRx(&USCI_A1_cnf);
    xSemaphoreGive(rb->busy_semaphore);
    return;
}

bool rb_disable_interrupts(ROCKBLOCK_t *rb) {
    if(xSemaphoreTake(rb->busy_semaphore, 20000 / portTICK_RATE_MS) == pdFALSE)
        return false;
    disableUSCIUartInterrupts(&USCI_A1_cnf);
    return true;
    //TODO: disable UART
}
