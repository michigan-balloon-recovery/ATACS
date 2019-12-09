# RockBLOCK9603 driver
RockBLOCK9603 driver for:
1. Sending messages from a microcontroller to the Iridium Satellite Network
2. Downloading and processing messages received from the Iridium Satellite Network

## Library Dependencies
1. FreeRTOS (semaphore and mutex support)
2. [Gustavo Litovsky's UART driver for MSP430](../uart/README.md) (modified to use [Frame-preserving ring buffer](../ring_buff/README.md))

## Hardware Resources
1. USCI A1
   1. Rx pin: P5.7
   2. Tx pin: P5.6
2. GPIO Pins
   1. RockBLOCK sleep pin: P8.4
   2. RockBLOCK ring pin: P8.0
   3. RockBLOCK network available pin: P8.1

## Usage
1. Set the RockBLOCK transmission frequency in milliseconds by using the #define in `./rockblock.h` (i.e. `#define RB_TRANSMIT_RATE_MS 60000`)
2. Set the RockBLOCK transmission retry frequency in milliseconds by using the #define in `./rockblock.h` (i.e. `#define RB_RETRY_RATE_MS 150000`)
3. Set the RockBLOCK maximum number of TX and RX retries by using the #defines in `./rockblock.h` (i.e. `#define RB_MAX_TX_RETRIES 5` and `#define RB_MAX_RX_RETRIES 5`)
4. Set buffer lengths to desired sizes in `./rockblock.h`
5. Register `task_rockblock()` with the FreeRTOS kernel (ex. `xTaskCreate(task_rockblock, "rb", 1024, NULL, 1, NULL);`)
6. Use `rb_init()` to initialize the RockBLOCK and get it ready to use.
7. Use the `rb_send_message()` function to send data to the Iridium network.
8. Use `rb_retrieve_message()` to download a message from the RockBLOCK to the microcontroller.
9. Use `rb_start_session()` to initiate a satellite communications session with the Iridium Network. This sends any queued messages and downloads any new messages sent to the RockBLOCK. Called automatically by `rb_send_message()`.
10. Use functions (`rb_disable_interrupts()`, `rb_enable_interrupts()`) to safely allow for critical sections in other regions of code.

## Example
This library has functions for basic use of the RockBLOCK for this MSP430 as well as more advanced control specific to this application. Here we go through how to use the RockBLOCK to send and receive data in its simplest form. Check `./rockblock.h` for more details on these and the other available functions.
1. **Instantiate a RockBLOCK struct**: Nearly all of the RockBLOCK functions need a RockBLOCK struct to be passed into them. It is important to never lose scope on the RockBLOCK struct or you may lose data. This is done via `ROCKBLOCK_t rb;`
2. **Initialize the RockBLOCK**: Run the initialization function before doing anything else. This will ensure that UART and the other important pins are properly set up. This is done via `rb_init(&rb);`. Note we are passing in a pointer to the ROCKBLOCK_t struct, not the object itself.
3. **Create your message**: Sending a message requires you to have first created a character array with all of the data inside of it and know how long it is. For example, `char msg[20] = 'testing;'` and `uint16_t len = strlen('testing');` 
4. **Create variables to hold function outputs**: We also need to create some variables to hold the response from the RockBLOCK. We need `bool msgSent = false;` to hold whether or not the message was actually sent. In addition, we need `int8_t msgReceived = 0;` to indicate message receive status and `int8_t msgsQueued = 0;` to hold how many messages are waiting on the network to be downloaded. Note that sending a message will also cause a message to be downloaded from the network if it is available.   
5. **Send the message**: Now we will send our message. Note that `msgSent`, `msgReceived`, and `msgsQueued` are all outputs of this function. Sending is done via `rb_send_message(&rb, msg, len, &msgSent, &msgReceived, &msgsQueued);`. If `msgSent` is true, then you successfully transmitted a message. If `msgReceived` is 1, then we received a message from the network. If 0, then we did not. If -1, there was an error when downloading the message. The variable `msgsQueued` is the number of messages on the network waiting to be downloaded.
6. **Retrieve a message if one was downloaded**: If `msgsReceived` is equal to 1, then a message was downloaded onto the RockBLOCK. You can retrieve this message from the RockBLOCK via `rb_retrieve_message(&rb);` If this function returns false, then the message was not successfully retrieved. You are free to retry this function later. The message can be found in the character array `rb.rx.buff`.
7. **Retrieve a message directly without sending anything**: It is possible to download messages from the network without sending a message. However, if you do this and there is no message to download, a credit will still be consumed. It is advised that you wait to download until you are sure there is a message available. You can use `msgsQueued` after a transmission or check the ring indicator on the RockBLOCK. Check the comments in `rockblock.h` for information about the ring indicator. To check your mailbox (download any messages), use `rb_check_mailbox(&rb, &msgsQueued)`. This function returns a uint8_t. It will be equal to -1 if there was an error in communication, 0 if there are no messages available (you lost a credit for nothing), and 1 if you downloaded a message. The variable `msgsQueued` tells you how many more messages are able to be downloaded to the RockBLOCK. To get the message from the RockBLOCK to the MSP430, follow step 6. Always retrieve the message from the RockBLOCK between every download from the network. The RockBLOCK can only hold one message at a time in its memory. Failure to retrieve the message between every download will result in the data being lost. 
