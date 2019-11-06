/*
 * rockblock.h
 *
 *  Created on: Nov 6, 2019
 *      Author: Scott Smith
 */

#ifndef SRC_ROCKBLOCK_ROCKBLOCK_H_
#define SRC_ROCKBLOCK_ROCKBLOCK_H_

#include <stdbool.h>
#include <stdint.h>

// initializes the RockBLOCK module and uart ports on the msp430 to what we need.
void rb_init();

// Queries and determines if we have a message on satellite ready to be received.
// If there is a message, it fills the rx buffer with the data from the message.
// This function will return true if there is a message, and false if there is no message.
// Consumes credit if no message was on satellite!
bool rb_check_mailbox();

// Check if we have message waiting for us.
// Does not consume a credit; this is done locally.
// Returns true if phone is ringing, otherwise false.
// Recommend to use this first, as it does not consume credits.
// Potentially possible for no ring to have been received but do have message.
bool rb_check_ring();

// Gets the signal strength our RockBLOCK has with the Iridium Network.
// This can take considerable time, so not always worth it to use this function.
// Included here in case you want it.
uint8_t rb_get_ssi();

// Sends a message over the Iridum Network from our RockBLOCK.
// Returns a bool indicating if the message was successfully sent or not.
// If there was a message waiting to be received, the message will be pulled into the RockBLOCK as well.
// This means we don't waste credits to check our mailbox if you are sending data at the same time.
// NOTE: Always pull any outstanding messages from the RockBLOCK into MSP430 RX buffer and process them
// before sending a message, because if we receive a new message data will be lost.
// We return values into the passed by reference arguments of this function.
// bool messageSent indicates if the message was successfully sent or not.
// bool messageReceived indicates if we received a message from the network.
void rb_send_message();

// Grabs message from the RockBLOCK.
// Must have already been downloaded onto RockBLOCK. Does not consume credits, as we are polling our RockBLOCK's memory.
void rb_retrieve_message();



#endif /* SRC_ROCKBLOCK_ROCKBLOCK_H_ */
