# Frame-preserving Ring Buffer
Ring buffer designed to prevent fragmented frames by independently storing byte and frame position.

## Library Dependencies
None

## Hardware Resources
None

## Usage
## Initialization
1. Create a global-scope array of type `uint8_t` or equivalent
2. Call `ring_buff_init()` and pass in a empty `ring_buff_t` instance and the byte array to use.

## Write to Buffer
1. Call `ring_buff_write_clear_packet()` at the beginning of a frame to clear out any partial frames
2. Call `ring_buff_write()` to add each byte of the frame
3. Call `ring_buff_write_finish_packet()` to mark the packet as finished. **Warning:** If the frame isn't marked as finished, it will be deleted when `ring_buff_write_clear_packet()` is called.

## Read from Buffer
1. Call `ring_buff_read_clear_packet()` at the beginning of a read to ensure the pointer is at the start of a frame
2. Call `ring_buff_read()` to read in each byte of the frame
3. Call `ring_buff_read_finish_packet()` when the End-Of-Frame (EOF) is detected to mark the frame as read. **Warning:** If the frame isn't marked as finished, the buffer will be unable to use the memory occupied by the current frame.
