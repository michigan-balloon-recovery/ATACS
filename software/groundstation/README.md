# ATACS Ground Station software
## Dependencies
1. Windows Operating System
2. Python3 Installed
3. Firefox browser installed
4. Gmail account without duo authentication (RockBLOCK messages must be routed to this address on their control panel)
5. RockBLOCK account with credits available (to send messages)
6. Google Maps API key

## Usage
1. Download `groundstation.py`. Place it into a folder, it will generate a few files while running. Open the script in a text editor, we will have to change a few lines of the software.
2. Fill in your RockBLOCK username, password, and imei inside of the `rb_send_message()` function in the Python code.
3. Fill in your Google Maps API key inside of the `process_rb_message()` function in the Python code.
4. Make sure Firefox is installed and added to your enviornmental variables. This isn't difficult to do, try looking up how to do this for your operating system.
5. Open `groundstation.py` from terminal. Enter your email username and password. This must be the email that has RockBLOCK data being routed to it.
6. Press 'g' to get and decode the most recent packet from the RockBLOCK.
7. Press 's' to send a message to the RockBLOCK. Press 'f' afterward to cut the FTU. Sending commands to other payloads via 'p' is not yet implemented.
8. After 'q' to quit the program.
9. The program will loop forever until you quit.
