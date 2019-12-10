# UART Driver
Genaric UART driver for the MSP430 by Gustavo Litovsky (Refer to copywrite notice in `./uart.c`).
Has been heavily modified to support Paul Young's [Frame-preserving ring buffer](../ring_buff/README.md) as well as adding callback functions for RX and TX operations.

## Library Dependencies
1. Paul Young's [Frame-preserving ring buffer](../ring_buff/README.md)

## Hardware Resources
configured by application drivers

## Usage

## Initialization
1. Create and initialize a `UARTConfig` struct, filling in *moduleName*, *PortNum*, *RxPinNum*, *TxPinNum*, *clkRate*, *baudRate*, *clkSrc*, *databits*, *parity*, and *stopbits* based on the application
2. Call `initUSCIUart()`, passing in the `UARTConfig` struct and any ring buffers being used. If a ring buffer isn't being used due to callback functions, a NULL pointer can be passed in instead.
3. Call `initUartRxCallback()` and/or `initUartTxCallback()` to initialize callback functions
   1. A function pointer to a callback of prototype `void callback_fcn(void *param, uint8_t datum)` should be passed in
   2. data can be returned from the callback function using the void pointer *params* that will be passed into the callback function

## RX
After initialization, the RX interrupt is automatically enabled. When the interrupt is triggered, the callback function will be called, if initialized. 
If no callback is defined, the interrupt will load the byte into the ring buffer and finish the frame.
If no callback or ring buffer is defined, nothing will happen.

## TX: Blocking
A byte array can be transmitted over UART without interrupts by using `uartSendDataBlocking()`. This will lock up the current task until the transmission is completed.
No callbacks or internal buffers are used for this transmission method.

## TX: Interrupts
Interrupts can be used to free the task for other operations while transmission is occuring. The driver supports callback functions and a Frame-preserving ring buffer.
1. Start the transmission using `uartSendDataInt()`. This enables TX interrupts and triggers the first interrupt with software.
2. After each byte is transmitted, an interrupt is triggered which calls the callback function, if defined.
3. If no callback function is defined, a single byte is read from the ring buffer (and transmitted) and the frame is marked as complete.
4. If no callback or ring buffer is defined, nothing will happen
